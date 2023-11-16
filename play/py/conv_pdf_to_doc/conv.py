import logging
import os
import sys

from pdf2docx import Converter
from docxcompose.composer import Composer
from docx import Document as Document_compose


def conv_pdf_to_docs(pdf_file, output_dir):
    """
    convert pdf to multiple docs
    :param pdf_file: pdf filepath
    :param output_dir: output dir
    :return:
    """
    cv = Converter(pdf_file)
    n_page = len(cv.fitz_doc)
    logging.info("total page: {}".format(n_page))
    once_conv_pages = 64
    n_steps = int(n_page / once_conv_pages)
    docs = []
    for i in range(n_steps + 1):
        start_page = i * once_conv_pages
        end_page = (i + 1) * once_conv_pages
        if end_page > n_page:
            end_page = n_page
        logging.info("covert pdf pages: [{}:{})".format(start_page, end_page))
        section_docx_file = "{}/section{:03}.docx".format(output_dir, i)
        docs.append(section_docx_file)
        cv.convert(section_docx_file, start=start_page, end=end_page)
    cv.close()
    return docs


def combine_word_documents(doc_list, output_file):
    """
    combime multiple words
    :param doc_list: list of words
    :param output_file: output filepath
    :return:
    """
    logging.info("start merge docx files")
    number_of_sections = len(doc_list)
    master = Document_compose(doc_list[0])
    composer = Composer(master)
    for i in range(1, number_of_sections):
        logging.info("append docx: {}".format(doc_list[i]))
        doc_temp = Document_compose(doc_list[i])
        composer.append(doc_temp)
    composer.save(output_file)


if __name__ == "__main__":
    if len(sys.argv) != 3:
        logging.error("Usage: {} <input pdf> <output docx>".format(sys.argv[0]))
        sys.exit(1)

    pdf_file = sys.argv[1]
    docx_file = sys.argv[2]

    # create output dir
    output_dir = os.path.dirname(docx_file)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir, exist_ok=True)

    # convert
    doc_list = conv_pdf_to_docs(pdf_file, output_dir)

    # combine
    combine_word_documents(doc_list, docx_file)
