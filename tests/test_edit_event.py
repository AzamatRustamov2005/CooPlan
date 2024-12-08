import pytest

from testsuite.databases import pgsql

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_no_token(service_client):
    headers = {}
    response = await service_client.put('/events/1', headers = headers, json = {})
    assert response.status == 401
    assert response.json() == {'message': 'Invalid or missing access token'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_no_event_id(service_client):
    token = 'session_token_participant1'
    headers = {"Authorization": token}
    response = await service_client.put('/events/', headers = headers, json = {})
    assert response.status == 400
    assert response.json() == {'message': 'Event ID is required'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_wrong_format_id(service_client):
    token = 'session_token_participant1'
    headers = {"Authorization": token}
    response = await service_client.put('/events/std', headers = headers, json = {})
    assert response.status == 400
    assert response.json() == {'message': 'Invalid Event ID format'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_wrong_event_id(service_client):
    token = 'session_token_participant1'
    headers = {"Authorization": token}
    response = await service_client.put('/events/7', headers = headers, json = {})
    assert response.status == 404
    assert response.json() == {'message': 'Event not found'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_not_organizer(service_client):
    token = 'session_token_participant1'
    headers = {"Authorization": token}
    response = await service_client.put('/events/4', headers = headers, json = {})
    assert response.status == 401
    assert response.json() == {'message': 'Unauthorized: User is not the organizer of this event'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_succesfull_deletion(service_client):
    token = 'session_token_organizer2'
    data = {
    "title": "Updated Event Title",
    "description": "Updated description of the event.",
    "is_active": False,
    "members_limit": 150,
    "start_datetime": "2024-12-20 10:00:00",
    "finish_datetime": "2024-12-20 18:00:00",
    "registration_deadline": "2024-12-19 23:59:59",
    "latitude": 37.7749,
    "longitude": -122.4194,
    "image_url": "http://example.com/updated_image.jpg"
    }
    headers = {"Authorization": token}
    response = await service_client.put('/events/4', headers = headers, json = data)
    assert response.status == 200
    assert response.json() == {'status' : 'updated'}