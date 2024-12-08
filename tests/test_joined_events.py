import pytest


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_joined_events(service_client):
    response = await service_client.get('/joined-events/123')
    assert response.status == 200
    data = response.json()
    events = data['joined_events']
    assert events[0]['title'] == 'Annual Tech Conference'


async def test_no_joined_events(service_client):
    response = await service_client.get('/joined-events/999')
    assert response.status == 400
    data = response.json()
    assert data['message'] == 'User not found.'
