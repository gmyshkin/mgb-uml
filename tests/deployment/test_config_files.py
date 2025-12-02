"""
Unit tests for configuration files (nginx.conf, supervisord.conf).
Tests syntax, structure, and required settings.
"""

import unittest
import re
import os
from pathlib import Path


class TestNginxConfig(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.root = Path(__file__).parent.parent.parent
        cls.nginx_conf_file = cls.root / "nginx.conf"

    def test_nginx_conf_exists(self):
        self.assertTrue(self.nginx_conf_file.exists(),
                       f"nginx.conf not found at {self.nginx_conf_file}")

    def test_nginx_conf_not_empty(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            self.assertTrue(len(content.strip()) > 0, "nginx.conf is empty")

    def test_nginx_conf_has_events_block(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('events', content, "nginx.conf must have 'events' block")

    def test_nginx_conf_has_http_block(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('http', content, "nginx.conf must have 'http' block")

    def test_nginx_conf_has_server_block(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('server {', content, "nginx.conf must have 'server' block")

    def test_nginx_conf_has_upstream_block(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('upstream', content, "nginx.conf should define upstream backend")

    def test_nginx_conf_has_ssl_certificate(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('ssl_certificate', content,
                         "nginx.conf should configure SSL certificates")

    def test_nginx_conf_has_ssl_key(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('ssl_certificate_key', content,
                         "nginx.conf should configure SSL key")

    def test_nginx_conf_has_proxy_pass(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('proxy_pass', content,
                         "nginx.conf should have proxy_pass for backend")

    def test_nginx_conf_has_http_redirect(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('return 301', content,
                         "nginx.conf should redirect HTTP to HTTPS")

    def test_nginx_conf_has_websocket_upgrade(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('Upgrade', content,
                         "nginx.conf should support WebSocket upgrade headers")

    def test_nginx_conf_has_docs_location(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('/docs/', content,
                         "nginx.conf should have /docs/ location for documentation")

    def test_nginx_conf_has_worker_connections(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('worker_connections', content,
                         "nginx.conf should configure worker_connections")

    def test_nginx_conf_braces_balanced(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            open_braces = content.count('{')
            close_braces = content.count('}')
            self.assertEqual(open_braces, close_braces,
                           f"Unbalanced braces: {open_braces} open, {close_braces} close")

    def test_nginx_conf_no_syntax_errors(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            if 'server_name' in content:
                self.assertIsNotNone(
                    re.search(r'server_name\s+[\w\.\-]+', content),
                    "server_name directive syntax invalid"
                )

    def test_nginx_conf_has_acme_challenge_location(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('/.well-known/acme-challenge/', content,
                         "nginx.conf should have ACME challenge location for Let's Encrypt")

    def test_nginx_conf_listen_ports(self):
        with open(self.nginx_conf_file, 'r') as f:
            content = f.read()
            self.assertRegex(content, r'listen\s+80', "nginx.conf should listen on port 80")
            self.assertRegex(content, r'listen\s+443\s+ssl', 
                           "nginx.conf should listen on port 443 with SSL")


class TestSupervisorConfig(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.root = Path(__file__).parent.parent.parent
        cls.supervisord_conf_file = cls.root / "supervisord.conf"

    def test_supervisord_conf_exists(self):
        self.assertTrue(self.supervisord_conf_file.exists(),
                       f"supervisord.conf not found at {self.supervisord_conf_file}")

    def test_supervisord_conf_not_empty(self):
        with open(self.supervisord_conf_file, 'r') as f:
            content = f.read()
            self.assertTrue(len(content.strip()) > 0, "supervisord.conf is empty")

    def test_supervisord_conf_has_supervisord_section(self):
        with open(self.supervisord_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('[supervisord]', content,
                         "supervisord.conf must have [supervisord] section")

    def test_supervisord_conf_nodaemon_true(self):
        with open(self.supervisord_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('nodaemon=true', content,
                         "supervisord should run in foreground mode (nodaemon=true)")

    def test_supervisord_conf_has_vncserver_program(self):
        with open(self.supervisord_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('[program:vncserver]', content,
                         "supervisord.conf should define vncserver program")

    def test_supervisord_conf_has_novnc_program(self):
        with open(self.supervisord_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('[program:novnc]', content,
                         "supervisord.conf should define novnc program")

    def test_supervisord_vncserver_has_command(self):
        with open(self.supervisord_conf_file, 'r') as f:
            content = f.read()
            # Find vncserver section and check for command
            vncserver_section = re.search(r'\[program:vncserver\](.*?)(?=\[|$)', content, re.DOTALL)
            self.assertIsNotNone(vncserver_section, "vncserver section not found")
            self.assertIn('command=', vncserver_section.group(1),
                         "vncserver program must have command directive")

    def test_supervisord_vncserver_has_autorestart(self):
        with open(self.supervisord_conf_file, 'r') as f:
            content = f.read()
            vncserver_section = re.search(r'\[program:vncserver\](.*?)(?=\[|$)', content, re.DOTALL)
            self.assertIsNotNone(vncserver_section)
            self.assertIn('autorestart=', vncserver_section.group(1),
                         "vncserver program should have autorestart directive")

    def test_supervisord_novnc_has_command(self):
        with open(self.supervisord_conf_file, 'r') as f:
            content = f.read()
            novnc_section = re.search(r'\[program:novnc\](.*?)(?=\[|$)', content, re.DOTALL)
            self.assertIsNotNone(novnc_section, "novnc section not found")
            self.assertIn('command=', novnc_section.group(1),
                         "novnc program must have command directive")

    def test_supervisord_vncserver_runs_as_user(self):
        with open(self.supervisord_conf_file, 'r') as f:
            content = f.read()
            vncserver_section = re.search(r'\[program:vncserver\](.*?)(?=\[|$)', content, re.DOTALL)
            self.assertIsNotNone(vncserver_section)
            self.assertIn('user=', vncserver_section.group(1),
                         "vncserver program should specify user")

    def test_supervisord_has_logfile(self):
        with open(self.supervisord_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('logfile=', content,
                         "supervisord should have logfile configuration")

    def test_supervisord_logfile_unbounded(self):
        with open(self.supervisord_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('logfile_maxbytes=0', content,
                         "supervisord logfile_maxbytes should be 0 for Docker (unbounded)")

    def test_supervisord_vncserver_has_environment_vars(self):
        with open(self.supervisord_conf_file, 'r') as f:
            content = f.read()
            vncserver_section = re.search(r'\[program:vncserver\](.*?)(?=\[|$)', content, re.DOTALL)
            self.assertIsNotNone(vncserver_section)
            self.assertIn('environment=', vncserver_section.group(1),
                         "vncserver program should set environment variables")

    def test_supervisord_vncserver_geometry_configured(self):
        with open(self.supervisord_conf_file, 'r') as f:
            content = f.read()
            self.assertIn('-geometry', content,
                         "VNC geometry should be configured")

    def test_supervisord_section_headers_valid(self):
        with open(self.supervisord_conf_file, 'r') as f:
            content = f.read()
            section_pattern = r'\[[\w:]+\]'
            sections = re.findall(section_pattern, content)
            self.assertTrue(len(sections) > 0, "No valid section headers found")
            for section in sections:
                self.assertRegex(section, section_pattern,
                               f"Invalid section header: {section}")

    def test_supervisord_no_invalid_directives(self):
        with open(self.supervisord_conf_file, 'r') as f:
            for line_num, line in enumerate(f, 1):
                line = line.strip()
                if line and not line.startswith(';') and not line.startswith('#') and not line.startswith('['):
                    self.assertIn('=', line,
                                 f"Line {line_num} doesn't have key=value format: {line}")


if __name__ == '__main__':
    unittest.main()
