import datetime
import os


class RepayRecord:
    def __init__(self):
        # 期数
        self.idx = 0

        # 是否为一次性冲还
        self.is_once_repay = False

        # 本期支付公积金
        self.repay_social_bal = 0
        self.repay_social_interest = 0

        # 本期支付商贷
        self.repay_commercial_bal = 0
        self.repay_commercial_interest = 0

        # 本期自付金额 (为负时代表可以累计公积金)
        self.extra_pay = 0

        # 本期末公积金余额
        self.social_fund_remain = 0

        # 本期末贷款余额
        self.social_debt_remain = 0
        self.commercial_debt_remain = 0


class Loan:
    def __init__(self):
        # 总贷款金额
        self.total_debt = 3050000

        # 公积金贷款金额
        self.social_debt = 1100000

        # 商业贷款金额
        self.commercial_debt = self.total_debt - self.social_debt

        # 公积金余额
        self.social_fund_bal = 439794.21 + 90000

        # 公积金每月入账
        self.social_fund_monthly = 8204 + 900

        # 公积金贷款利率
        self.social_interest_rate = 0.031

        # 商业贷款利率
        self.commercial_interest_rate = 0.0455

        # 分期
        self.social_months = 18 * 12
        self.commercial_months = 20 * 12

        # 一次性冲还
        self.once_repay = 0.0
        self.once_repay_idx = -1

    def cal_repay_eq_principal(self):
        """
        等额本金
        每期还款额 = 还款本金 / 贷款总期数 + (本金 - 已归还的本金累计额) * 周期利率

        例如:
        贷款 20 万, 贷款期为 10 年, 每季度归还
        每季度等额归还本金 = 200000 / (10 * 4) = 5000
        第 1 个季度利息 = 200000 * (年利率 / 4)
        第 2 个季度利息 = (200000 - (5000 * 1)) * (年利率 / 4)
        第 3 个季度利息 = (200000 - (5000 * 2)) * (年利率 / 4)
        ...
        第 40 个季度利息 = (200000 - (5000 * 39)) * (年利率 / 4)
        :return:
        """
        months = max(self.social_months, self.commercial_months)

        social_debt_remain = self.social_debt
        social_repay_per_period = self.social_debt / self.social_months

        commercial_debt_remain = self.commercial_debt
        commercial_repay_per_period = self.commercial_debt / self.commercial_months

        social_fund_remain = self.social_fund_bal

        total_repay_social_bal = 0
        total_repay_social_interest = 0
        total_repay_commercial_bal = 0
        total_repay_commercial_interest = 0

        # 判断 0 的精度
        precision = 0.01

        records = []

        for i in range(months):
            if social_debt_remain < precision:
                curr_repay_social_bal = 0.0
                curr_repay_social_interest = 0.0
            else:
                curr_repay_social_interest = social_debt_remain * self.social_interest_rate / 12
                curr_repay_social_bal = social_repay_per_period
                social_debt_remain -= social_repay_per_period
                total_repay_social_interest += curr_repay_social_interest
                total_repay_social_bal += curr_repay_social_bal
                if social_debt_remain <= 0:
                    social_debt_remain = 0

            if commercial_debt_remain < precision:
                curr_repay_commercial_bal = 0.0
                curr_repay_commercial_interest = 0.0
            else:
                curr_repay_commercial_interest = commercial_debt_remain * self.commercial_interest_rate / 12
                curr_repay_commercial_bal = commercial_repay_per_period
                commercial_debt_remain -= commercial_repay_per_period
                total_repay_commercial_interest += curr_repay_commercial_interest
                total_repay_commercial_bal += curr_repay_commercial_bal
                if commercial_debt_remain <= 0:
                    commercial_debt_remain = 0

            netting = curr_repay_social_bal \
                      + curr_repay_social_interest \
                      + curr_repay_commercial_bal \
                      + curr_repay_commercial_interest \
                      - self.social_fund_monthly

            if netting > precision:
                if social_fund_remain > precision:
                    if social_fund_remain > netting:
                        social_fund_remain -= netting
                        netting = 0
                    else:
                        netting -= social_fund_remain
                        social_fund_remain = 0
            else:
                social_fund_remain += (-netting)

            record = RepayRecord()
            record.idx = i
            record.repay_social_bal = curr_repay_social_bal
            record.repay_social_interest = curr_repay_social_interest
            record.repay_commercial_bal = curr_repay_commercial_bal
            record.repay_commercial_interest = curr_repay_commercial_interest
            record.extra_pay = netting
            record.social_fund_remain = social_fund_remain
            record.social_debt_remain = social_debt_remain
            record.commercial_debt_remain = commercial_debt_remain
            records.append(record)

            # 一次性冲还
            if self.once_repay > 1.0 and self.once_repay_idx == i + 1:
                record = RepayRecord()
                record.idx = i
                record.is_once_repay = True
                record.repay_commercial_bal = self.once_repay
                record.extra_pay = self.once_repay
                record.social_fund_remain = social_fund_remain
                record.social_debt_remain = social_debt_remain
                record.commercial_debt_remain = commercial_debt_remain - self.once_repay
                records.append(record)

                total_repay_commercial_bal += self.once_repay
                commercial_debt_remain -= self.once_repay
                commercial_repay_per_period = commercial_debt_remain / (self.commercial_months - i - 1)

        print("总计: {:.2f}, 本金: {:.2f}, 利息: {:.2f}".format(
            total_repay_social_bal + total_repay_social_interest + total_repay_commercial_bal + total_repay_commercial_interest,
            total_repay_social_bal + total_repay_commercial_bal,
            total_repay_social_interest + total_repay_commercial_interest))
        return records

    def cal_repay_eq_principal_andinterest(self):
        """
        等额本息
        每个月还款金额时相同的, 原理就是把要贷款的本金 S 加上之后产生的利息 I, 然后吧本金和利息之和 (S + I) 除以还款期限
        每月还款额 = 贷款金额 * (月利率 * (1 + 月利率)^还款月数) / ((1 + 月利率)^还款月数 - 1)

        公式推导:
        假设贷款总金额为 S, 月利率为 r, 贷款期限为 n, 每期还款总额(包含本金和利息)为 x
        第 1 期还款后, 欠款总金额为:
            D1 = S * (1 + r) - x
        第 2 期还款后, 欠款总金额为:
            D2 = D1 * (1 + r) - x
               = S * (1 + r)^2 - x * (1 + r) - x
               = S * (1 + r)^2 - x * (1 + (1 + r))
        第 3 期还款后, 欠款总金额为:
            D3 = D2 * (1 + r) - x
               = S * (1 + r)^3 - x * (1 + (1 + r) + (1 + r)^2)
        。。。
        第 k 期还款后, 欠款总金额为 D[k]
            D[k] = D[k-1] * (1 + r) - x
                 = S * (1 + r)^k - x * (1 + (1 + r) + (1 + r)^2 + ... + (1 + r)^(k - 1))
            明显: 1 + (1 + r) + (1 + r)^2 + ... + (1 + r)^(k - 1) 为一个等比数列
            D[k] = S * (1 + r)^k - x * ((1 + r)^k - 1) / r
        当 k 为最后一期时, 则 D[k] = 0
            0 = S * (1 + r)^k - x * ((1 + r)^k - 1) / r
            x * ((1 + r)^k - 1) / r = S * (1 + r)^k
            x = r * S * (1 + r)^k / ((1 + r)^k - 1)
        :return:
        """
        months = max(self.social_months, self.commercial_months)

        social_r = self.social_interest_rate / 12
        repay_social = \
            self.social_debt * (social_r * (1 + social_r) ** self.social_months) / \
            ((1 + social_r) ** self.social_months - 1)

        commercial_r = self.commercial_interest_rate / 12
        repay_commercial = \
            self.commercial_debt * (commercial_r * (1 + commercial_r) ** self.commercial_months) / \
            ((1 + commercial_r) ** self.commercial_months - 1)

        social_debt_remain = self.social_debt
        commercial_debt_remain = self.commercial_debt

        social_fund_remain = self.social_fund_bal

        total_repay_social_bal = 0
        total_repay_social_interest = 0
        total_repay_commercial_bal = 0
        total_repay_commercial_interest = 0

        # 判断 0 的精度
        precision = 0.01

        records = []

        for i in range(months):
            if social_debt_remain < precision:
                curr_repay_social_bal = 0.0
                curr_repay_social_interest = 0.0
            else:
                prev_social_debt_remain = social_debt_remain
                social_debt_remain = social_debt_remain * (1 + social_r) - repay_social
                curr_repay_social_bal = prev_social_debt_remain - social_debt_remain
                curr_repay_social_interest = repay_social - curr_repay_social_bal
                total_repay_social_interest += curr_repay_social_interest
                total_repay_social_bal += curr_repay_social_bal
                if social_debt_remain <= 0:
                    social_debt_remain = 0

            if commercial_debt_remain < precision:
                curr_repay_commercial_bal = 0.0
                curr_repay_commercial_interest = 0.0
            else:
                prev_commercial_debt_remain = commercial_debt_remain
                commercial_debt_remain = commercial_debt_remain * (1 + commercial_r) - repay_commercial
                curr_repay_commercial_bal = prev_commercial_debt_remain - commercial_debt_remain
                curr_repay_commercial_interest = repay_commercial - curr_repay_commercial_bal
                total_repay_commercial_interest += curr_repay_commercial_interest
                total_repay_commercial_bal += curr_repay_commercial_bal
                if commercial_debt_remain <= 0:
                    commercial_debt_remain = 0

            netting = curr_repay_social_bal \
                      + curr_repay_social_interest \
                      + curr_repay_commercial_bal \
                      + curr_repay_commercial_interest \
                      - self.social_fund_monthly

            if netting > precision:
                if social_fund_remain > precision:
                    if social_fund_remain > netting:
                        social_fund_remain -= netting
                        netting = 0
                    else:
                        netting -= social_fund_remain
                        social_fund_remain = 0
            else:
                social_fund_remain += (-netting)

            record = RepayRecord()
            record.idx = i
            record.repay_social_bal = curr_repay_social_bal
            record.repay_social_interest = curr_repay_social_interest
            record.repay_commercial_bal = curr_repay_commercial_bal
            record.repay_commercial_interest = curr_repay_commercial_interest
            record.extra_pay = netting
            record.social_fund_remain = social_fund_remain
            record.social_debt_remain = social_debt_remain
            record.commercial_debt_remain = commercial_debt_remain
            records.append(record)

            # 一次性冲还
            if self.once_repay > 1.0 and self.once_repay_idx == i + 1:
                record = RepayRecord()
                record.idx = i
                record.is_once_repay = True
                record.repay_commercial_bal = self.once_repay
                record.extra_pay = self.once_repay
                record.social_fund_remain = social_fund_remain
                record.social_debt_remain = social_debt_remain
                record.commercial_debt_remain = commercial_debt_remain - self.once_repay
                records.append(record)

                total_repay_commercial_bal += self.once_repay
                commercial_debt_remain -= self.once_repay

                remain_commercial_month = self.commercial_months - i - 1
                repay_commercial = \
                    commercial_debt_remain * (commercial_r * (1 + commercial_r) ** remain_commercial_month) / \
                    ((1 + commercial_r) ** remain_commercial_month - 1)

        print("总计: {:.2f}, 本金: {:.2f}, 利息: {:.2f}".format(
            total_repay_social_bal + total_repay_social_interest + total_repay_commercial_bal + total_repay_commercial_interest,
            total_repay_social_bal + total_repay_commercial_bal,
            total_repay_social_interest + total_repay_commercial_interest))
        return records

    def output_records(self, records, prefix_name):
        """
        打印结果
        :param records:
        :param prefix_name:
        :return:
        """
        outdir = "output"
        if not os.path.exists(outdir):
            os.makedirs(outdir, exist_ok=True)
        # tzinfo = datetime.timezone(datetime.timedelta(hours=8))
        # curdate = datetime.datetime.now(tz=tzinfo)
        # filename = "{}_{}.csv".format(prefix_name, curdate.strftime("%Y%m%dT%H%M%S"))
        filename = "{}.csv".format(prefix_name)
        filepath = os.path.join(outdir, filename)
        with open(filepath, "w") as f:
            f.write(
                "期数,月供,公积金余额,自付金额,商贷还款,公积金还款,商贷本金,商贷利息,公积金还款本金,公积金还款利息,商贷余额,公积金贷款余额\n")
            for record in records:
                if record.is_once_repay:
                    idx = "一次性冲还"
                else:
                    idx = record.idx + 1
                f.write("{},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f}\n".format(
                    idx,
                    record.repay_social_bal + record.repay_social_interest +
                    record.repay_commercial_bal + record.repay_commercial_interest,
                    record.social_fund_remain,
                    record.extra_pay,
                    record.repay_commercial_bal + record.repay_commercial_interest,
                    record.repay_social_bal + record.repay_social_interest,
                    record.repay_commercial_bal, record.repay_commercial_interest,
                    record.repay_social_bal, record.repay_social_interest,
                    record.commercial_debt_remain,
                    record.social_debt_remain
                ))


if __name__ == "__main__":
    loan = Loan()
    loan.total_debt = 3000000
    loan.social_debt = 1000000
    loan.commercial_debt = loan.total_debt - loan.social_debt
    loan.social_fund_bal = 500000
    loan.social_fund_monthly = 9000
    loan.social_interest_rate = 0.031
    loan.commercial_interest_rate = 0.0455
    loan.social_months = 20 * 12
    loan.commercial_months = 20 * 12

    loan.once_repay = 0.0
    loan.once_repay_idx = -1

    print("等额本金")
    records = loan.cal_repay_eq_principal()
    loan.output_records(records=records, prefix_name="等额本金")

    print("等额本息")
    records = loan.cal_repay_eq_principal_andinterest()
    loan.output_records(records=records, prefix_name="等额本息")

    loan.once_repay = 1000000.0
    loan.once_repay_idx = 39
    print("等额本金_包含冲抵")
    records = loan.cal_repay_eq_principal()
    loan.output_records(records=records, prefix_name="等额本金_包含冲抵")

    loan.once_repay = 1000000.0
    loan.once_repay_idx = 53
    print("等额本息_包含冲抵")
    records = loan.cal_repay_eq_principal_andinterest()
    loan.output_records(records=records, prefix_name="等额本息_包含冲抵")
