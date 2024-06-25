import streamlit as st


if __name__ == "__main__":
    st.write("## 多列布局")

    col1, col2, col3 = st.columns(3)

    with col1:
        st.write("Col 1")

    with col2:
        st.write("Col 2")

    with col3:
        st.write("Col 3")

    st.divider()

    st.write("## 多列布局(控制比例)")

    col1, col2, col3 = st.columns([1, 2, 3])

    with col1:
        st.write("Col 1")

    with col2:
        st.write("Col 2")

    with col3:
        st.write("Col 3")
