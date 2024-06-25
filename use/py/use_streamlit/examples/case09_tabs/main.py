import streamlit as st


if __name__ == "__main__":
    tab1, tab2, tab3 = st.tabs(["三体", "银河系搭车客指南", "..."])

    with tab1:
        st.write("地球往事")
        st.write("黑暗森林")
        st.write("死神永生")

    with tab2:
        st.write("宇宙尽头的餐厅")
        st.write("生命、宇宙及万事万物")
        st.write("掰掰，谢谢你们的鱼啦")
        st.write("基本无害")
        st.write("还有一件事...")

    with tab3:
        st.write("...")
