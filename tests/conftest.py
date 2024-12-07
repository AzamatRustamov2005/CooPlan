import pathlib

import pytest

import aiohttp

from testsuite.databases.pgsql import discover


pytest_plugins = ['pytest_userver.plugins.postgresql']


@pytest.fixture(scope='session')
def service_source_dir():
    """Path to root directory service."""
    return pathlib.Path(__file__).parent.parent


@pytest.fixture(scope='session')
def initial_data_path(service_source_dir):
    """Path for find files with data"""
    return [
        service_source_dir / 'postgresql/data',
    ]


@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    """Create schemas databases for tests"""
    databases = discover.find_schemas(
        'cooplan',  # service name that goes to the DB connection
        [service_source_dir.joinpath('postgresql/schemas')],
    )
    return pgsql_local_create(list(databases.values()))


@pytest.fixture(name='register')
async def register(service_client):
    async def _register(username: str, password: str):
        json = {
            'username': username,
            'password': password
        }
        response = await service_client.post(
            '/register',
            json=json
        )
        return response
    return _register


@pytest.fixture(name='login')
async def login(service_client):
    async def _login(email: str, password: str):
        json = {
            'username': username,
            'password': password
        }
        headers = {
            'Content-Type': 'multipart/form-data; boundary=',
        }
        response = await service_client.post(
            '/login',
            json=json
        )
        return response
    return _login
