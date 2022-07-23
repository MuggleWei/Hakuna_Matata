#!/bin/bash

source venv/bin/activate

pytest --cov=fooproj --cov=barproj tests/
