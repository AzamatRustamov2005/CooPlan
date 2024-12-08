import pytest

from testsuite.databases import pgsql


# Start the tests via `make test-debug` or `make test-release`
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_events(service_client):
    response = await service_client.get('/v1/events')
    assert response.status == 200
    assert response.json() != {}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_events_by_id(service_client):
    response = await service_client.get('/v1/event/1')
    assert response.status == 200
    assert response.json != {}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_response_to_join_approve(service_client):
    token = 'asdf'
    headers = {"X-Ya-User-Ticket": token}
    data = {'user_id': 1, 'event_id': 1, 'status': "accepted"}
    response = await service_client.post('/v1/response_to_join', headers = headers, json = data)
    assert response.status == 200

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_response_to_join_deny(service_client):
    token = 'asdf'
    headers = {"X-Ya-User-Ticket": token}
    data = {'user_id': 1, 'event_id': 1, 'status': "rejected"}
    response = await service_client.post('/v1/response_to_join', headers = headers, json = data)
    assert response.status == 200
