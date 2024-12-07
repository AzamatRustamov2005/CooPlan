import pytest

from testsuite.databases import pgsql


# Start the tests via `make test-debug` or `make test-release`
async def test_first_time_users(register):
    response = await register(
        username='test_username',
        password='strong_password',
    )
    assert response.status == 200
