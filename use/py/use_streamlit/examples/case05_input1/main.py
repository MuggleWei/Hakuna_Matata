import streamlit as st


if __name__ == "__main__":
    name = st.text_input("Name: ")
    pwd = st.text_input("Password: ", type="password")
    submitted = st.button("login")
    if submitted:
        if name:
            st.write("name: {}".format(name))
        if pwd:
            st.write("password: {}".format(pwd))
