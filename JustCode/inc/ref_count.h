template<typename T>
class RefCount
{
public:
    RefCount()
        : used_(nullptr)
        , obj_(nullptr)
    {}
    RefCount(const RefCount &ref)
        : used_(ref.used_)
        , obj_(ref.obj_)
    {
        if (used_ != nullptr)
        {
            ++*used_;
        }
    }
    RefCount(T *obj)
        : obj_(obj)        
    {
        if (obj_ != nullptr)
        {
            used_ = new std::size_t(1);
        }
        else
        {
            used_ = nullptr;
        }
    }
    ~RefCount()
    {
        if (used_ != nullptr && --*used_ == 0)
        {
            delete used_;
            delete obj_;
        }
    }

    RefCount& operator=(const RefCount &ref)
    {
        if (ref.used_ != nullptr)
        {
            ++*ref.used_;
        }

        if (used_ != nullptr && --*used_ == 0)
        {
            delete used_;
            delete obj_;
        }

        used_ = ref.used_;
        obj_ = ref.obj_;

        return *this;
    }

private:
    std::size_t *used_;
    T *obj_;
};