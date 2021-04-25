import setuptools

with open("readme.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

with open("requirements-dev.txt", "r", encoding="utf-8") as f:
    requires = []
    for line in f:
        if len(line) == 0:
            continue
        requires.append(line.strip())

setuptools.setup(
    name="mugglefoo",
    version="0.0.1",
    author="Muggle Wei",
    author_email="mugglewei@gmail.com",
    description="example for packaging python projects",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="",
    package_dir={"": "src"},
    packages=setuptools.find_packages(where="src"),
    python_requires=">=3.6",
    install_requires=requires,
)
