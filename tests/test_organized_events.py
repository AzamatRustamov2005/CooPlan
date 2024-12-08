import pytest


@pytest.mark.pgsql('cooplan', files=['initial_data.sql'])
async def test_organized_events(service_client, mock_user_data):
    response = await service_client.get('/v1/organized-events?id=123')
    assert response.status == 200
    data = response.json()
    events = data['organized_events']
    assert len(events) == 1
    assert events[0]['title'] == 'Event 1'
    assert events[0]['organizer_id'] == '123'


async def test_no_organized_events(service_client):
    response = await service_client.get('/v1/organized-events?id=999')
    assert response.status == 400
    data = response.json()
    assert data['message'] == 'User not found.'
