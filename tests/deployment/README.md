# TikZiT Deployment Infrastructure Tests

Comprehensive unit tests for Docker, deployment scripts, and infrastructure configuration files.

## Overview

These tests validate the web deployment infrastructure for TikZiT, ensuring:

- **Docker Compose Configurations** are syntactically valid and properly configured
- **Nginx Configuration** is correct for reverse proxy and SSL
- **Supervisor Configuration** properly manages services
- **Shell Scripts** have no syntax errors and implement required functionality
- **Dockerfile** follows best practices and includes all necessary components

## Test Files

### `test_docker_compose.py`
Tests for `docker-compose.yml` and `docker-compose.prod.yml`:
- ✅ Valid YAML syntax
- ✅ Required fields (version, services, volumes, networks)
- ✅ Service definitions (image, ports, volumes, environment)
- ✅ Network and volume configuration
- ✅ Environment variable usage
- ✅ Service dependencies

**23 test cases** covering all aspects of Docker Compose configuration.

### `test_config_files.py`
Tests for `nginx.conf` and `supervisord.conf`:

#### nginx.conf Tests (18 cases)
- ✅ Valid Nginx syntax
- ✅ Required blocks (events, http, server, upstream)
- ✅ SSL configuration (certificates, keys)
- ✅ Proxy setup (proxy_pass, WebSocket upgrades)
- ✅ HTTPS redirect and ACME challenge
- ✅ Documentation serving
- ✅ Port configuration (80, 443)
- ✅ Balanced braces

#### supervisord.conf Tests (18 cases)
- ✅ Valid configuration syntax
- ✅ Required sections ([supervisord], [program:*])
- ✅ Foreground mode (nodaemon=true)
- ✅ Program configuration (command, autorestart, user)
- ✅ Environment variables
- ✅ VNC geometry settings
- ✅ Proper log configuration

**36 test cases** for configuration file validation.

### `test_shell_scripts.py`
Tests for deployment and management scripts:

#### Script Syntax Validation
- ✅ `entrypoint.sh` - Docker entry point script
- ✅ `deploy.sh` - Initial server setup
- ✅ `multi_user_manager.sh` - User container management
- ✅ `update_and_redeploy.sh` - Update deployment
- ✅ `publish_all.sh` - Release publishing

#### entrypoint.sh Tests (6 cases)
- ✅ Bash syntax validation
- ✅ Error handling (set -e)
- ✅ VNC_PASSWORD validation
- ✅ VNC directory creation
- ✅ Password file creation

#### deploy.sh Tests (4 cases)
- ✅ Bash syntax validation
- ✅ Directory creation
- ✅ Docker network creation
- ✅ Docker volume creation
- ✅ Environment file checking

#### multi_user_manager.sh Tests (6 cases)
- ✅ Username validation (alphanumeric)
- ✅ Environment file checking
- ✅ Docker container creation
- ✅ Nginx configuration generation
- ✅ Nginx reload

#### update_and_redeploy.sh Tests (3 cases)
- ✅ Docker image pulling
- ✅ Documentation updates
- ✅ Container restart logic

#### publish_all.sh Tests (10 cases)
- ✅ Version handling
- ✅ Documentation generation (Doxygen)
- ✅ Git operations (commit, tag, push)
- ✅ Docker build and push
- ✅ Version validation

#### Dockerfile Tests (20 cases)
- ✅ Multi-stage build structure
- ✅ Required FROM statements
- ✅ Dependency installation
- ✅ Apt cache cleanup
- ✅ Application building
- ✅ Port exposure
- ✅ User creation
- ✅ Configuration copying
- ✅ Entrypoint setup

**52 test cases** for shell scripts and Dockerfile.

## Running the Tests

### Prerequisites

```bash
pip install pyyaml
```

### Run All Deployment Tests

```bash
cd /root/tikzit/tests/deployment
python3 run_tests.py
```

### Run Specific Test Module

```bash
# Test Docker Compose files
python3 -m unittest test_docker_compose -v

# Test Configuration files
python3 -m unittest test_config_files -v

# Test Shell scripts
python3 -m unittest test_shell_scripts -v
```

### Run Specific Test Class

```bash
# Test only nginx.conf
python3 -m unittest test_config_files.TestNginxConfig -v

# Test only supervisord.conf
python3 -m unittest test_config_files.TestSupervisorConfig -v

# Test only Docker Compose production config
python3 -m unittest test_docker_compose.TestDockerCompose.test_docker_compose_prod_yml_valid_yaml -v
```

## Total Test Coverage

- **111 unit tests** across deployment infrastructure
- **Docker Compose**: 23 tests
- **Configuration Files**: 36 tests  
- **Scripts**: 52 tests

## Expected Output

```
======================================================================
TikZiT Deployment Infrastructure Tests
======================================================================

✅ Loaded tests from test_docker_compose
✅ Loaded tests from test_config_files
✅ Loaded tests from test_shell_scripts

======================================================================
Running Tests...
======================================================================

test_docker_compose_yml_exists ... ok
test_docker_compose_yml_valid_yaml ... ok
test_docker_compose_has_version ... ok
...

======================================================================
Test Summary
======================================================================
Tests run: 111
Successes: 111
Failures: 0
Errors: 0

✅ All deployment tests passed!
```

## Test Categories

### 1. **Syntax Validation**
- YAML parsing and validation
- Shell script syntax checking
- Configuration file format validation

### 2. **Configuration Verification**
- Required fields and sections
- Proper service definitions
- Network and volume setup
- Environment variables

### 3. **Deployment Logic**
- Script error handling
- User validation
- Docker operations
- File management

### 4. **Security & Best Practices**
- Non-root user creation
- SSL certificate handling
- Password validation
- Apt cache cleanup

## Integration with CI/CD

### GitHub Actions Example

```yaml
- name: Run Deployment Tests
  run: |
    cd tests/deployment
    python3 run_tests.py
```

### GitLab CI Example

```yaml
deployment_tests:
  stage: test
  image: python:3.9
  script:
    - pip install pyyaml
    - cd tests/deployment
    - python3 run_tests.py
```

## Maintenance

When adding new deployment files or scripts:

1. Create corresponding test file
2. Add test class for new component
3. Implement 3-5 test cases per feature
4. Run full test suite: `python3 run_tests.py`
5. Verify all tests pass before committing

## Common Issues & Solutions

### "pyyaml not found"
```bash
pip install pyyaml
```

### "docker command not found in script"
Some tests check for docker command usage. This is normal for deployment scripts.

### "Permission denied" on shell script tests
Scripts don't need to be executable for syntax validation.

### Tests failing on Windows/WSL
YAML and shell syntax tests should work on all platforms. Some specific features may differ.

## Future Enhancements

- [ ] Container runtime testing (start/stop verification)
- [ ] Volume mounting validation
- [ ] Network connectivity tests
- [ ] Nginx configuration validation with `nginx -t`
- [ ] Performance benchmarking for Docker builds
- [ ] Integration tests with actual Docker daemon
