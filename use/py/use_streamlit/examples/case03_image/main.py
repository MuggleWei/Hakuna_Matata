import streamlit as st
import requests

from io import BytesIO


@st.cache_data
def get_image(url):
    """
    requests image and return BytesIO
    """
    try:
        r = requests.get(url)
        return BytesIO(r.content)
    except Exception as e:
        print("failed get image: {}".format(e))
        return None


if __name__ == "__main__":
    url = "https://avatars.githubusercontent.com/u/4484994"
    img = get_image(url)
    if img:
        st.image(img, width=400)
    else:
        st.write("failed download image: {}".format(url))
