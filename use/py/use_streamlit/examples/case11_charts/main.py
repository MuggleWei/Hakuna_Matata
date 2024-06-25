import streamlit as st
import numpy as np


if __name__ == "__main__":
    # generate 10 row and 4 columns
    data = np.random.randn(10, 4)
    st.line_chart(data)
