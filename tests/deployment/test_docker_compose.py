"""
Unit tests for docker-compose configuration files.
Tests YAML syntax, structure, and required fields.
"""

import unittest
import yaml
import os
import sys
from pathlib import Path


class TestDockerCompose(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.root = Path(__file__).parent.parent.parent
        cls.docker_compose_file = cls.root / "docker-compose.yml"
        cls.docker_compose_prod_file = cls.root / "docker-compose.prod.yml"

    def test_docker_compose_yml_exists(self):
        self.assertTrue(self.docker_compose_file.exists(), 
                       f"docker-compose.yml not found at {self.docker_compose_file}")

    def test_docker_compose_prod_yml_exists(self):
        self.assertTrue(self.docker_compose_prod_file.exists(),
                       f"docker-compose.prod.yml not found at {self.docker_compose_prod_file}")

    def test_docker_compose_yml_valid_yaml(self):
        with open(self.docker_compose_file, 'r') as f:
            try:
                yaml.safe_load(f)
            except yaml.YAMLError as e:
                self.fail(f"docker-compose.yml is not valid YAML: {e}")

    def test_docker_compose_prod_yml_valid_yaml(self):
        with open(self.docker_compose_prod_file, 'r') as f:
            try:
                yaml.safe_load(f)
            except yaml.YAMLError as e:
                self.fail(f"docker-compose.prod.yml is not valid YAML: {e}")

    def test_docker_compose_has_version(self):
        with open(self.docker_compose_file, 'r') as f:
            config = yaml.safe_load(f)
            self.assertIn('version', config, "docker-compose.yml missing 'version' field")
            self.assertIsNotNone(config['version'], "version field should not be None")

    def test_docker_compose_prod_has_version(self):
        with open(self.docker_compose_prod_file, 'r') as f:
            config = yaml.safe_load(f)
            self.assertIn('version', config, "docker-compose.prod.yml missing 'version' field")
            self.assertIsNotNone(config['version'], "version field should not be None")

    def test_docker_compose_has_services(self):
        with open(self.docker_compose_file, 'r') as f:
            config = yaml.safe_load(f)
            self.assertIn('services', config, "docker-compose.yml missing 'services' field")
            self.assertTrue(len(config['services']) > 0, "No services defined")

    def test_docker_compose_prod_has_services(self):
        with open(self.docker_compose_prod_file, 'r') as f:
            config = yaml.safe_load(f)
            self.assertIn('services', config, "docker-compose.prod.yml missing 'services' field")
            self.assertTrue(len(config['services']) > 0, "No services defined in prod")

    def test_docker_compose_services_have_images(self):
        with open(self.docker_compose_file, 'r') as f:
            config = yaml.safe_load(f)
            for service_name, service in config['services'].items():
                if 'build' not in service:
                    self.assertIn('image', service, 
                                 f"Service '{service_name}' must have 'image' or 'build'")

    def test_docker_compose_prod_tikzit_service_exists(self):
        with open(self.docker_compose_prod_file, 'r') as f:
            config = yaml.safe_load(f)
            self.assertIn('tikzit_default', config['services'],
                         "docker-compose.prod.yml must have 'tikzit_default' service")

    def test_docker_compose_prod_nginx_service_exists(self):
        with open(self.docker_compose_prod_file, 'r') as f:
            config = yaml.safe_load(f)
            self.assertIn('nginx', config['services'],
                         "docker-compose.prod.yml must have 'nginx' service")

    def test_docker_compose_prod_has_networks(self):
        with open(self.docker_compose_prod_file, 'r') as f:
            config = yaml.safe_load(f)
            self.assertIn('networks', config, 
                         "docker-compose.prod.yml should define networks")
            self.assertIn('tikzit_net', config['networks'],
                         "docker-compose.prod.yml must have 'tikzit_net' network")

    def test_docker_compose_prod_has_volumes(self):
        with open(self.docker_compose_prod_file, 'r') as f:
            config = yaml.safe_load(f)
            self.assertIn('volumes', config,
                         "docker-compose.prod.yml should define volumes")

    def test_docker_compose_prod_services_on_network(self):
        with open(self.docker_compose_prod_file, 'r') as f:
            config = yaml.safe_load(f)
            for service_name in ['tikzit_default', 'nginx']:
                self.assertIn('networks', config['services'][service_name],
                             f"Service '{service_name}' must be on a network")

    def test_docker_compose_version_format(self):
        with open(self.docker_compose_file, 'r') as f:
            config = yaml.safe_load(f)
            version = str(config['version'])
            self.assertTrue(len(version) > 0, "Version should not be empty")

    def test_docker_compose_prod_nginx_has_volumes(self):
        with open(self.docker_compose_prod_file, 'r') as f:
            config = yaml.safe_load(f)
            nginx_service = config['services']['nginx']
            self.assertIn('volumes', nginx_service,
                         "nginx service should have volumes for config and certs")

    def test_docker_compose_prod_certbot_has_volumes(self):
        with open(self.docker_compose_prod_file, 'r') as f:
            config = yaml.safe_load(f)
            self.assertIn('certbot', config['services'],
                         "docker-compose.prod.yml should have certbot service")
            certbot = config['services']['certbot']
            self.assertIn('volumes', certbot,
                         "certbot service must have volumes for SSL certs")

    def test_docker_compose_dev_tikzit_service_exists(self):
        with open(self.docker_compose_file, 'r') as f:
            config = yaml.safe_load(f)
            self.assertIn('tikzit', config['services'],
                         "docker-compose.yml must have 'tikzit' service")

    def test_docker_compose_tikzit_has_ports(self):
        with open(self.docker_compose_file, 'r') as f:
            config = yaml.safe_load(f)
            tikzit_service = config['services']['tikzit']
            self.assertIn('ports', tikzit_service,
                         "tikzit service should expose ports")

    def test_docker_compose_tikzit_exposes_port_8080(self):
        with open(self.docker_compose_file, 'r') as f:
            config = yaml.safe_load(f)
            ports = config['services']['tikzit'].get('ports', [])
            port_8080_found = any('8080' in str(p) for p in ports)
            self.assertTrue(port_8080_found,
                          "tikzit service should expose port 8080 for noVNC")

    def test_docker_compose_prod_env_variable_usage(self):
        with open(self.docker_compose_prod_file, 'r') as f:
            content = f.read()
            self.assertIn('${VNC_PASSWORD}', content,
                         "docker-compose.prod.yml should use environment variables")

    def test_docker_compose_prod_nginx_conf_mount(self):
        with open(self.docker_compose_prod_file, 'r') as f:
            config = yaml.safe_load(f)
            nginx_volumes = config['services']['nginx'].get('volumes', [])
            nginx_conf_mounted = any('nginx.conf' in str(v) for v in nginx_volumes)
            self.assertTrue(nginx_conf_mounted,
                          "nginx service should mount nginx.conf")


if __name__ == '__main__':
    unittest.main()
