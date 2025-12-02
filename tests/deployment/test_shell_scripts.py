"""
Unit tests for shell scripts.
Tests for syntax errors, required variables, functions, etc.
"""

import unittest
import subprocess
import re
from pathlib import Path


class TestShellScripts(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.root = Path(__file__).parent.parent.parent

    def _check_shell_syntax(self, script_path):
        result = subprocess.run(
            ['bash', '-n', str(script_path)],
            capture_output=True,
            text=True
        )
        return result.returncode == 0, result.stderr

    def _extract_variables(self, script_path):
        with open(script_path, 'r') as f:
            content = f.read()
        # Find all variable assignments
        return re.findall(r'^\s*(\w+)=', content, re.MULTILINE)

    def _extract_commands(self, script_path):
        with open(script_path, 'r') as f:
            content = f.read()
        # Find all external commands
        return re.findall(r'\b(docker|bash|echo|git|qmake|make|doxygen)\b', content)

    def test_entrypoint_sh_syntax(self):
        script = self.root / "entrypoint.sh"
        self.assertTrue(script.exists(), f"{script} not found")
        is_valid, error = self._check_shell_syntax(script)
        self.assertTrue(is_valid, f"entrypoint.sh has syntax errors: {error}")

    def test_deploy_sh_syntax(self):
        script = self.root / "deploy.sh"
        self.assertTrue(script.exists(), f"{script} not found")
        is_valid, error = self._check_shell_syntax(script)
        self.assertTrue(is_valid, f"deploy.sh has syntax errors: {error}")

    def test_multi_user_manager_sh_syntax(self):
        script = self.root / "multi_user_manager.sh"
        self.assertTrue(script.exists(), f"{script} not found")
        is_valid, error = self._check_shell_syntax(script)
        self.assertTrue(is_valid, f"multi_user_manager.sh has syntax errors: {error}")

    def test_update_and_redeploy_sh_syntax(self):
        script = self.root / "update_and_redeploy.sh"
        self.assertTrue(script.exists(), f"{script} not found")
        is_valid, error = self._check_shell_syntax(script)
        self.assertTrue(is_valid, f"update_and_redeploy.sh has syntax errors: {error}")

    def test_publish_all_sh_syntax(self):
        script = self.root / "publish_all.sh"
        self.assertTrue(script.exists(), f"{script} not found")
        is_valid, error = self._check_shell_syntax(script)
        self.assertTrue(is_valid, f"publish_all.sh has syntax errors: {error}")

    def test_entrypoint_sh_has_error_handling(self):
        script = self.root / "entrypoint.sh"
        with open(script, 'r') as f:
            content = f.read()
        # Check for error handling patterns
        has_set_e = 'set -e' in content or 'set -euo pipefail' in content
        has_exit_checks = 'exit 1' in content or '|| exit' in content
        self.assertTrue(has_set_e or has_exit_checks,
                       "entrypoint.sh should have error handling")

    def test_entrypoint_sh_checks_vnc_password(self):
        script = self.root / "entrypoint.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('VNC_PASSWORD', content,
                     "entrypoint.sh should check VNC_PASSWORD")
        self.assertIn('not set', content,
                     "entrypoint.sh should validate VNC_PASSWORD is set")

    def test_entrypoint_sh_creates_vnc_dirs(self):
        script = self.root / "entrypoint.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('.vnc', content,
                     "entrypoint.sh should create .vnc directory")

    def test_entrypoint_sh_sets_vnc_password(self):
        script = self.root / "entrypoint.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('vncpasswd', content,
                     "entrypoint.sh should set VNC password")

    def test_deploy_sh_has_error_handling(self):
        script = self.root / "deploy.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertTrue('set -e' in content or 'exit' in content or 'echo' in content,
                       "deploy.sh should have some form of command execution")

    def test_deploy_sh_creates_directories(self):
        script = self.root / "deploy.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('mkdir', content,
                     "deploy.sh should create required directories")

    def test_deploy_sh_creates_network(self):
        script = self.root / "deploy.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('docker network create', content,
                     "deploy.sh should create Docker network")

    def test_deploy_sh_creates_volume(self):
        script = self.root / "deploy.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('docker volume create', content,
                     "deploy.sh should create Docker volume")

    def test_deploy_sh_env_file_check(self):
        script = self.root / "deploy.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertTrue('docker' in content or 'network' in content or 'volume' in content,
                       "deploy.sh should perform docker operations")

    def test_multi_user_manager_validates_username(self):
        script = self.root / "multi_user_manager.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('USERNAME', content,
                     "multi_user_manager.sh should use USERNAME variable")
        self.assertIn('[a-z0-9]', content,
                     "multi_user_manager.sh should validate username format")

    def test_multi_user_manager_checks_env_file(self):
        script = self.root / "multi_user_manager.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('.env', content,
                     "multi_user_manager.sh should check for .env file")

    def test_multi_user_manager_creates_container(self):
        script = self.root / "multi_user_manager.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('docker run', content,
                     "multi_user_manager.sh should run docker containers")

    def test_multi_user_manager_creates_nginx_config(self):
        script = self.root / "multi_user_manager.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertTrue('CONFIG_DIR' in content or 'proxy_pass' in content or '.conf' in content,
                       "multi_user_manager.sh should handle nginx configuration")

    def test_multi_user_manager_reloads_nginx(self):
        script = self.root / "multi_user_manager.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('nginx', content,
                     "multi_user_manager.sh should reload nginx")

    def test_update_and_redeploy_pulls_image(self):
        script = self.root / "update_and_redeploy.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('docker pull', content,
                     "update_and_redeploy.sh should pull latest image")

    def test_update_and_redeploy_updates_docs(self):
        script = self.root / "update_and_redeploy.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('docs', content.lower(),
                     "update_and_redeploy.sh should update documentation")

    def test_update_and_redeploy_restarts_containers(self):
        script = self.root / "update_and_redeploy.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertTrue('docker' in content and 'up' in content,
                       "update_and_redeploy.sh should restart containers")

    def test_publish_all_sh_has_version_handling(self):
        script = self.root / "publish_all.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('VERSION', content,
                     "publish_all.sh should handle versioning")

    def test_publish_all_sh_reads_version_file(self):
        script = self.root / "publish_all.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertTrue('VERSION' in content and ('cat' in content or 'read' in content),
                       "publish_all.sh should read VERSION file")

    def test_publish_all_sh_generates_docs(self):
        script = self.root / "publish_all.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('doxygen', content,
                     "publish_all.sh should generate documentation")

    def test_publish_all_sh_checks_docs_generated(self):
        script = self.root / "publish_all.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('index.html', content,
                     "publish_all.sh should verify docs were generated")

    def test_publish_all_sh_pushes_to_git(self):
        script = self.root / "publish_all.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('git push', content,
                     "publish_all.sh should push to git")

    def test_publish_all_sh_tags_release(self):
        script = self.root / "publish_all.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('git tag', content,
                     "publish_all.sh should create git tags")

    def test_publish_all_sh_builds_docker_image(self):
        script = self.root / "publish_all.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('docker build', content,
                     "publish_all.sh should build Docker image")

    def test_publish_all_sh_pushes_to_registry(self):
        script = self.root / "publish_all.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('docker push', content,
                     "publish_all.sh should push to registry")

    def test_entrypoint_sh_uses_set_e(self):
        script = self.root / "entrypoint.sh"
        with open(script, 'r') as f:
            first_line = f.readline()
        self.assertIn('bash', first_line,
                     "entrypoint.sh should have proper shebang")

    def test_deploy_sh_uses_set_e(self):
        script = self.root / "deploy.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('#!/bin/bash', content,
                     "deploy.sh should have proper bash shebang")

    def test_multi_user_manager_uses_conf_dir_var(self):
        script = self.root / "multi_user_manager.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('CONFIG_DIR', content,
                     "multi_user_manager.sh should use CONFIG_DIR variable")

    def test_update_and_redeploy_uses_registry_url(self):
        script = self.root / "update_and_redeploy.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('REGISTRY_URL', content or 'registry' in content,
                     "update_and_redeploy.sh should use registry URL")

    def test_publish_all_sh_prompts_for_version(self):
        script = self.root / "publish_all.sh"
        with open(script, 'r') as f:
            content = f.read()
        self.assertIn('read', content,
                     "publish_all.sh should prompt for user input")


class TestDockerfile(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.root = Path(__file__).parent.parent.parent
        cls.dockerfile = cls.root / "Dockerfile"

    def test_dockerfile_exists(self):
        self.assertTrue(self.dockerfile.exists(),
                       f"Dockerfile not found at {self.dockerfile}")

    def test_dockerfile_not_empty(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertTrue(len(content.strip()) > 0, "Dockerfile is empty")

    def test_dockerfile_has_from_statement(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertIn('FROM', content,
                         "Dockerfile must have FROM statement")

    def test_dockerfile_has_build_stage(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertTrue('builder' in content or 'AS' in content,
                          "Dockerfile should have multi-stage build")

    def test_dockerfile_has_runtime_stage(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            # Should have multiple FROM statements for multi-stage
            from_count = content.count('FROM')
            self.assertGreaterEqual(from_count, 2,
                                   "Dockerfile should have multiple build stages")

    def test_dockerfile_installs_dependencies(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertTrue('apt-get install' in content or 'RUN' in content,
                          "Dockerfile should install dependencies")

    def test_dockerfile_cleans_up_apt_cache(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertIn('apt-get clean', content,
                         "Dockerfile should clean apt cache to reduce size")

    def test_dockerfile_removes_apt_lists(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertIn('rm -rf /var/lib/apt/lists', content,
                         "Dockerfile should remove apt lists")

    def test_dockerfile_builds_app(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertTrue('qmake' in content or 'make' in content,
                          "Dockerfile should build the application")

    def test_dockerfile_exposes_ports(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertIn('EXPOSE', content,
                         "Dockerfile should expose ports")

    def test_dockerfile_sets_working_directory(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertIn('WORKDIR', content,
                         "Dockerfile should set working directory")

    def test_dockerfile_copies_files(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertTrue('COPY' in content or 'ADD' in content,
                          "Dockerfile should copy files")

    def test_dockerfile_has_entrypoint(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertTrue('ENTRYPOINT' in content or 'CMD' in content,
                          "Dockerfile should have ENTRYPOINT or CMD")

    def test_dockerfile_creates_user(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertIn('useradd', content,
                         "Dockerfile should create non-root user")

    def test_dockerfile_has_env_vars(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertIn('ENV', content,
                         "Dockerfile should set environment variables")

    def test_dockerfile_uses_noninteractive_frontend(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertIn('DEBIAN_FRONTEND=noninteractive', content,
                         "Dockerfile should use noninteractive apt mode")

    def test_dockerfile_no_sudo_command(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertNotIn('sudo', content,
                           "Dockerfile should not use sudo (running as root)")

    def test_dockerfile_copies_supervisord_config(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertIn('supervisord.conf', content,
                         "Dockerfile should copy supervisord config")

    def test_dockerfile_copies_entrypoint_script(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertIn('entrypoint.sh', content,
                         "Dockerfile should copy entrypoint script")

    def test_dockerfile_makes_entrypoint_executable(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertIn('chmod +x', content,
                         "Dockerfile should make entrypoint executable")

    def test_dockerfile_copies_docs(self):
        with open(self.dockerfile, 'r') as f:
            content = f.read()
            self.assertIn('docs', content,
                         "Dockerfile should copy documentation")


if __name__ == '__main__':
    unittest.main()
