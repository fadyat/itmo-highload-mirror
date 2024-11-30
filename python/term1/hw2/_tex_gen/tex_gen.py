from typing import List, Optional


def generate_document(
    *content: str,
    document_class: str = "article",
    packages: Optional[List[str]] = None,
) -> str:
    """
    Generate a LaTeX document from the given content.

    :param document_class: The class of the document.
    :param packages: The packages to include in the document.
    :param content: The content of the document.
    :return: The LaTeX code of the document.
    """

    if packages is None:
        packages = []

    packages_code = "\n".join(
        r"\usepackage{" + package + "}"
        for package in packages
    )

    return r"""
\documentclass{""" + document_class + r"""}
""" + packages_code + r"""
\begin{document}
""" + "\n".join(content) + r"""
\end{document}
"""


def generate_table(
    table: List[List[str]],
) -> str:
    """
    Generate a LaTeX table from the given content.

    :param table: The content of the table.
    :return: The LaTeX code of the table.
    """

    table_code = "\n".join(
        " & ".join(row) + r" \\"
        for row in table
    )

    return r"""
\begin{table}
\begin{tabular}{|""" + "c|" * len(table[0]) + r"""}
\hline
""" + table_code + r"""
\hline
\end{tabular}
\end{table}
"""


def generate_image(
    image_path: str,
    caption: Optional[str] = None,
) -> str:
    """
    Generate a LaTeX image from the given image path and caption.

    :param image_path: The path to the image.
    :param caption: The caption of the image.
    :return: The LaTeX code of the image.
    """

    return r"""
\begin{figure}
\includegraphics{""" + image_path + r"""}
""" + (r"\caption{" + caption + "}\n" if caption is not None else "") + r"""
\end{figure}
"""
