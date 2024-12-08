import pytest

from testsuite.databases import pgsql

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_no_token(service_client):
    headers = {}
    response = await service_client.delete('/events/', headers = headers, json = {})
    assert response.status == 400
    assert response.json() == {'message': 'Invalid or missing access token'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_no_event_id(service_client):
    token = 'session_token_participant1'
    headers = {"X-Ya-User-Ticket": token}
    response = await service_client.delete('/events/', headers = headers, json = {})
    assert response.status == 400
    assert response.json() == {'message': 'Event ID is required'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_wrong_event_id(service_client):
    token = 'session_token_participant1'
    headers = {"X-Ya-User-Ticket": token}
    response = await service_client.delete('/events/7', headers = headers, json = {})
    assert response.status == 404
    assert response.json() == {'message': 'Event not found'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_not_organizer(service_client):
    token = 'session_token_participant1'
    headers = {"X-Ya-User-Ticket": token}
    response = await service_client.delete('/events/4', headers = headers, json = {})
    assert response.status == 401
    assert response.json() == {'message': 'Unauthorized: User is not the organizer of this event'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_succesfull_deletion(service_client):
    token = 'session_token_organizer2'
    headers = {"X-Ya-User-Ticket": token}
    response = await service_client.delete('/events/4', headers = headers, json = {})
    assert response.status == 200
    assert response.json() == {'status' : 'deleted'}


