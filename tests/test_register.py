import pytest

from testsuite.databases import pgsql


# Start the tests via `make test-debug` or `make test-release`
async def test_register(register):
    response = await register(
        username='kacok5',
        password='qwerty',
    )
    assert response.status == 200
    assert response.json() != {}


async def test_login(service_client):
    json = {
        'username': 'kacok5',
        'password': 'qwerty'
    }
    register_response = await service_client.post(
        '/users/register',
        json=json
    )

    assert register_response.status == 200

    login_response = await service_client.post(
        '/users/login',
        json=json
    )
    assert login_response.status == 200
