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
        with aiohttp.MultipartWriter('form-data') as data:
            if username:
                payload = aiohttp.payload.StringPayload(username)
                payload.set_content_disposition('form-data', name='username')
                data.append_payload(payload)
            if password:
                payload = aiohttp.payload.StringPayload(password)
                payload.set_content_disposition('form-data', name='password')
                data.append_payload(payload)
        headers = {
            'Content-Type': 'multipart/form-data; boundary=' + data.boundary,
        }
        response = await service_client.post(
            '/v1/register-user',
            data=data,
            headers=headers
        )
        return response
    return _register


@pytest.fixture(name='login')
async def login(service_client):
    async def _login(email: str, password: str):
        with aiohttp.MultipartWriter('form-data') as data:
            if email:
                payload = aiohttp.payload.StringPayload(email)
                payload.set_content_disposition('form-data', name='email')
                data.append_payload(payload)
            if password:
                payload = aiohttp.payload.StringPayload(password)
                payload.set_content_disposition('form-data', name='password')
                data.append_payload(payload)
        headers = {
            'Content-Type': 'multipart/form-data; boundary=' + data.boundary,
        }
        response = await service_client.post(
            '/login',
            data=data,
            headers=headers
        )
        return response
    return _login
