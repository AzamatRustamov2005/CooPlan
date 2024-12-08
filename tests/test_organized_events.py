import pytest


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_organized_events(service_client):
    response = await service_client.get('/organized-events/1')
    assert response.status == 200
    data = response.json()
    events = data['organized_events']
    assert len(events) == 1
    assert events[0]['title'] == 'Annual Tech Conference'
    assert events[0]['organizer_id'] == 1


async def test_no_organized_events(service_client):
    response = await service_client.get('/organized-events/999')
    assert response.status == 400
    data = response.json()
    assert data['message'] == 'User not found.'
