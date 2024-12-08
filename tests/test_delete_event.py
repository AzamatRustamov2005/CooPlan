import pytest

from testsuite.databases import pgsql

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_no_token(service_client):
    token = 'session_token_participant1'
    headers = {}
    data = {}
    response = await service_client.delete('/delete_event', headers = headers, json = data)
    assert response.status == 401
    assert response.json() == {'message': 'Invalid or missing access token'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_no_event_id(service_client):
    token = 'session_token_participant1'
    headers = {"X-Ya-User-Ticket": token}
    data = {}
    response = await service_client.delete('/delete_event', headers = headers, json = data)
    assert response.status == 400
    assert response.json() == {'message': 'Event ID is required'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_wrong_event_id(service_client):
    token = 'session_token_participant1'
    headers = {"X-Ya-User-Ticket": token}
    data = {'event_id': 7}
    response = await service_client.delete('/delete_event', headers = headers, json = data)
    assert response.status == 404
    assert response.json() == {'message': 'Event not found'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_not_organizer(service_client):
    token = 'session_token_participant1'
    headers = {"X-Ya-User-Ticket": token}
    data = {'event_id': 2}
    response = await service_client.delete('/delete_event', headers = headers, json = data)
    assert response.status == 401
    assert response.json() == {'message': 'Unauthorized: User is not the organizer of this event'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_succesfull_deletion(service_client):
    token = 'session_token_organizer2'
    headers = {"X-Ya-User-Ticket": token}
    data = {'event_id': 2}
    response = await service_client.delete('/delete_event', headers = headers, json = data)
    assert response.status == 200
    assert response.json() == {'status' : 'deleted'}


