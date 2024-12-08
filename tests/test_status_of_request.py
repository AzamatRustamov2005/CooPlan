import pytest

from testsuite.databases import pgsql

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_no_token(service_client):
    headers = {"X-Ya-User-Ticket": ''}
    data = {'event_id': 3}
    response = await service_client.get('/status_of_request', headers = headers, json = data)
    assert response.status == 401

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_no_event_id(service_client):
    token = 'session_token_participant2'
    headers = {"X-Ya-User-Ticket": token}
    data = {}
    response = await service_client.get('/status_of_request', headers = headers, json = data)
    assert response.status == 400

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_pending(service_client):
    token = 'session_token_participant1'
    headers = {"X-Ya-User-Ticket": token}
    data = {'event_id': 3}
    response = await service_client.get('/status_of_request', headers = headers, json = data)
    assert response.status == 200
    assert response.json() == {'status' : 'pending'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_join(service_client):
    token = 'session_token_participant2'
    headers = {"X-Ya-User-Ticket": token}
    data = {'event_id': 5}
    response = await service_client.get('/status_of_request', headers = headers, json = data)
    assert response.status == 200
    assert response.json() == {'status': 'join'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_accepted(service_client):
    token = 'session_token_participant1'
    headers = {"X-Ya-User-Ticket": token}
    data = {'event_id': 4}
    response = await service_client.get('/status_of_request', headers = headers, json = data)
    assert response.status == 200
    assert response.json() == {'status' : 'accepted'}