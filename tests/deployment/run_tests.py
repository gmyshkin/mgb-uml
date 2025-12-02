#!/usr/bin/env python3
"""
Test runner for deployment infrastructure tests.
Runs all deployment tests and generates a report.
"""

import sys
import unittest
from pathlib import Path

def run_deployment_tests():
    """Run all deployment tests and report results."""
    
    test_dir = Path(__file__).parent
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    
    test_modules = [
        'test_docker_compose',
        'test_config_files',
        'test_shell_scripts',
    ]
    
    print("=" * 70)
    print("TikZiT Deployment Infrastructure Tests")
    print("=" * 70)
    print()
    
    for module_name in test_modules:
        try:
            module = __import__(module_name)
            module_tests = loader.loadTestsFromModule(module)
            suite.addTests(module_tests)
            print(f"✅ Loaded tests from {module_name}")
        except ImportError as e:
            print(f"❌ Failed to load {module_name}: {e}")
    
    print()
    print("=" * 70)
    print("Running Tests...")
    print("=" * 70)
    print()
    
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    print()
    print("=" * 70)
    print("Test Summary")
    print("=" * 70)
    print(f"Tests run: {result.testsRun}")
    print(f"Successes: {result.testsRun - len(result.failures) - len(result.errors)}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    print()
    
    if result.wasSuccessful():
        print("✅ All deployment tests passed!")
        return 0
    else:
        print("❌ Some tests failed. See details above.")
        return 1

if __name__ == '__main__':
    sys.exit(run_deployment_tests())
