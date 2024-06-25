import streamlit as st
import pandas as pd


if __name__ == "__main__":
    data = {
        "name": ["foo", "bar"],
        "age": [18, 22]
    }
    df = pd.DataFrame(data)

    st.write("## table dict (不可交互)")
    st.table(data=data)

    st.write("## table Dataframe (不可交互)")
    st.table(data=df)

    st.divider()

    st.write("## dataframe dict (可交互)")
    st.dataframe(data=data)

    st.write("## dataframe DataFrame (可交互)")
    st.dataframe(data=df)
