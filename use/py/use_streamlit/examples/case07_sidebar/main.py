import streamlit as st


if __name__ == "__main__":
    st.write("## 侧边栏")

    add_selectbox = st.sidebar.selectbox(
        "How wold you like to be contacted?",
        ("Email", "Home phone", "Mobile phone")
    )

    with st.sidebar:
        search_txt = st.text_input("Search")
    if search_txt:
        st.write("Search content: {}".format(search_txt))
