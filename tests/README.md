# TikZiT Test Suite

Comprehensive testing infrastructure for TikZiT including unit tests for core data structures and deployment infrastructure.

## Overview

The TikZiT test suite includes:

1. **Qt/C++ Unit Tests** - Core data structures (Node, Edge, Parser, etc.)
2. **Deployment Infrastructure Tests** - Docker, scripts, and configuration validation

## Directory Structure

```
tests/
├── README.md                    # This file
├── requirements.txt             # Python dependencies
├── deployment/                  # Infrastructure & deployment tests
│   ├── __init__.py
│   ├── README.md               # Deployment tests documentation
│   ├── run_tests.py            # Test runner
│   ├── test_docker_compose.py  # Docker Compose validation
│   ├── test_config_files.py    # Nginx & Supervisor config tests
│   └── test_shell_scripts.py   # Shell script validation
└── src/test/                    # Qt unit tests (in src/test/)
    ├── testmain.cpp
    ├── testnode.h              # NEW: Node class tests
    ├── testnode.cpp            # NEW: Node class tests
    ├── testedge.h              # NEW: Edge class tests
    ├── testedge.cpp            # NEW: Edge class tests
    ├── testdelimitedstringvalidator.h  # NEW: String validator tests
    ├── testdelimitedstringvalidator.cpp # NEW: String validator tests
    ├── testparser.h
    ├── testparser.cpp
    ├── testtikzoutput.h
    └── testtikzoutput.cpp
```

## Test Summary

### C++ Unit Tests (src/test/)
- **testnode.cpp**: 9 tests for Node class
- **testedge.cpp**: 12 tests for Edge class  
- **testdelimitedstringvalidator.cpp**: 8 tests for string validation
- **testparser.cpp**: 6 tests for TikZ parser
- **testtikzoutput.cpp**: 4 tests for output generation
- **testtest.cpp**: 2 basic tests

**Total: 41 C++ unit tests**

### Deployment Tests (tests/deployment/)
- **test_docker_compose.py**: 23 tests
- **test_config_files.py**: 36 tests
- **test_shell_scripts.py**: 52 tests

**Total: 111 deployment tests**

## Quick Start

### Running C++ Unit Tests

```bash
cd /root/tikzit
qmake -config test tikzit.pro
make
./UnitTests
```

Expected output:
```
All tests passed!
```

### Running Deployment Tests

```bash
cd /root/tikzit/tests/deployment

# Install dependencies
pip install -r ../requirements.txt

# Run all tests
python3 run_tests.py

# Or run specific tests
python3 -m unittest test_docker_compose -v
```

## Detailed Test Coverage

### C++ Tests

#### Node Class Tests (testnode.cpp) - 9 tests
- ✅ Constructor defaults
- ✅ Point getter/setter
- ✅ Name getter/setter
- ✅ Label getter/setter
- ✅ Data getter/setter
- ✅ Style name getter/setter
- ✅ Copy functionality
- ✅ isBlankNode() method
- ✅ tikzLine getter/setter

#### Edge Class Tests (testedge.cpp) - 12 tests
- ✅ Constructor basics
- ✅ Source/target accessors
- ✅ Self-loop detection
- ✅ Straight edge detection
- ✅ Data getter/setter
- ✅ Anchor getter/setter
- ✅ Edge node functionality
- ✅ Copy functionality
- ✅ Bend angles
- ✅ Weight property
- ✅ tikzLine property
- ✅ Style name getter/setter

#### DelimitedStringValidator Tests (testdelimitedstringvalidator.cpp) - 8 tests
- ✅ Acceptable strings
- ✅ Intermediate strings
- ✅ Invalid strings
- ✅ Fixup unclosed braces
- ✅ Fixup already valid
- ✅ Escaped braces handling
- ✅ Nested braces
- ✅ Mixed content

#### Parser Tests (testparser.cpp) - 6 tests
- ✅ Empty graph parsing
- ✅ Node graph parsing
- ✅ Edge graph parsing
- ✅ Edge node parsing
- ✅ Edge bend parsing
- ✅ Bounding box parsing

#### Output Tests (testtikzoutput.cpp) - 4 tests
- ✅ Escape functionality
- ✅ Data output
- ✅ Empty graph output
- ✅ Graph from TikZ
- ✅ Bounding box output

### Deployment Tests

#### Docker Compose (23 tests)
- ✅ File existence
- ✅ YAML validation
- ✅ Version field
- ✅ Services definition
- ✅ Service configuration
- ✅ Networks setup
- ✅ Volumes setup
- ✅ Environment variables

#### Configuration Files (36 tests)
- ✅ nginx.conf: 18 tests
  - Events, http, server blocks
  - SSL configuration
  - Proxy settings
  - Port configuration
  
- ✅ supervisord.conf: 18 tests
  - Supervisord section
  - Program definitions
  - Environment setup
  - Log configuration

#### Shell Scripts (52 tests)
- ✅ Syntax validation
- ✅ Error handling
- ✅ Variable usage
- ✅ Command execution
- ✅ File operations
- ✅ Docker operations
- ✅ Git operations

## Building and Testing Workflow

### 1. Build the Application
```bash
cd /root/tikzit
qmake tikzit.pro
make
```

### 2. Run C++ Unit Tests
```bash
qmake -config test tikzit.pro
make
./UnitTests
```

### 3. Run Deployment Tests
```bash
cd tests/deployment
pip install -r ../requirements.txt
python3 run_tests.py
```

### 4. Continuous Integration

Create `.github/workflows/tests.yml`:
```yaml
name: Tests

on: [push, pull_request]

jobs:
  unit-tests:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install Qt
        run: sudo apt-get install -y qt6-base-dev qmake6
      - name: Build tests
        run: |
          qmake -config test tikzit.pro
          make
      - name: Run C++ tests
        run: ./UnitTests
      
  deployment-tests:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Set up Python
        uses: actions/setup-python@v2
        with:
          python-version: '3.9'
      - name: Install dependencies
        run: pip install -r tests/requirements.txt
      - name: Run deployment tests
        run: |
          cd tests/deployment
          python3 run_tests.py
```

## Test Best Practices

### Adding New C++ Tests

1. Create header file in `src/test/testfeature.h`
2. Create implementation in `src/test/testfeature.cpp`
3. Add test class include to `testmain.cpp`
4. Add instantiation and execution to `main()` in `testmain.cpp`
5. Update `tikzit.pro` with new files
6. Run: `qmake -config test && make && ./UnitTests`

### Adding New Deployment Tests

1. Create test file in `tests/deployment/test_something.py`
2. Follow naming convention: `test_*.py`
3. Create test class: `class TestSomething(unittest.TestCase)`
4. Implement test methods: `def test_feature_name(self)`
5. Run: `python3 -m unittest test_something -v`

## Troubleshooting

### C++ Tests

**Problem**: `qmake: command not found`
```bash
# Ubuntu/Debian
sudo apt-get install qmake6

# macOS
brew install qt6
```

**Problem**: `UnitTests: command not found`
```bash
# Make sure you built the tests
qmake -config test tikzit.pro
make
```

### Deployment Tests

**Problem**: `ModuleNotFoundError: No module named 'yaml'`
```bash
pip install PyYAML
```

**Problem**: Bash syntax errors in scripts
The tests use `bash -n` to validate syntax. Ensure scripts are:
- Readable
- Valid bash syntax
- Have proper shebangs

## Contributing

When submitting code:

1. ✅ All C++ tests pass
2. ✅ All deployment tests pass
3. ✅ New code has test coverage
4. ✅ No regressions in existing tests

## Test Metrics

- **Total Tests**: 152
  - C++ Tests: 41
  - Deployment Tests: 111

- **Coverage Areas**:
  - Data structures (Node, Edge, Style, etc.)
  - Parsing and output
  - Docker and containerization
  - Deployment scripts
  - Configuration files

## Resources

- [Qt Test Framework](https://doc.qt.io/qt-6/qttest-index.html)
- [Python unittest](https://docs.python.org/3/library/unittest.html)
- [Docker Best Practices](https://docs.docker.com/develop/dev-best-practices/)
- [Bash Strict Mode](http://redsymbol.net/articles/unofficial-bash-strict-mode/)
