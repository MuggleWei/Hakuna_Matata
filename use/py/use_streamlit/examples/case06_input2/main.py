import streamlit as st


if __name__ == "__main__":
    st.write("## 数字输入框: number_input")

    money = st.number_input("Money: ")
    st.write("Your assets: {}".format(money))

    age = st.number_input("Age: ", step=1)
    st.write("Your age: {}".format(age))

    level = st.number_input("Level", value=20, min_value=0, max_value=99, step=1)
    st.write("Your Level: {}".format(level))

    st.divider()

    st.write("## 多行文本框: text_area")
    paragraph = st.text_area("多行内容: ")

    st.divider()

    st.write("## 单选按钮: radio")

    choice1 = st.radio("选择难度", ["简单", "中等", "困难"])
    st.write("选择: {}".format(choice1))

    choice2 = st.radio("选择难度(带默认值)", ["简单", "中等", "困难"], index=1)
    st.write("选择: {}".format(choice2))

    choice3 = st.radio("选择难度(默认不选择任何)", ["简单", "中等", "困难"], index=None)
    if choice3 is None:
        st.write("没有任何选择")
    else:
        st.write("选择: {}".format(choice3))

    st.divider()

    st.write("## 复选框: checkbox")
    checked = st.checkbox("同意以上条款")
    if checked:
        st.write("同意")
    else:
        st.write("不同意")

    st.divider()

    st.write("## 下拉框: selectbox")
    role = st.selectbox("选择角色", ["孙悟空", "猪八戒", "沙悟净"])
    if role:
        st.write("选择的角色: {}".format(role))

    st.divider()

    st.write("## 多选下拉框: multiselect")
    languages = st.multiselect(
        "选择几个编程语言",
        [ "c", "python", "go", "c++", "Java", "c#", "Javascript"])
    if languages:
        st.write(",".join(languages))

    st.divider()

    st.write("## 滑块: slider")
    attack_val = st.slider("Attack", value=500, min_value=1, max_value=999, step=1)
    if attack_val:
        st.write("攻击力: {}".format(attack_val))

    defence_val = st.slider("Defence", value=500, min_value=1, max_value=999, step=1)
    if defence_val:
        st.write("防御力: {}".format(defence_val))

    st.divider()

    st.write("## 文件上传: file_uploader")
    uploaded_file = st.file_uploader("上传文件", type=["csv", "json", "txt"])
    if uploaded_file:
        st.write("filename: {}".format(uploaded_file.name))
        bytes_data = uploaded_file.read()
        st.write(bytes_data)
