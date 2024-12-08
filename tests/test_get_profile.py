import pytest


@pytest.mark.pgsql('cooplan', files=['initial_data.sql'])
async def test_get_profile(service_client, mock_user_data):
    response = await service_client.get('/get-profile?id=123')
    assert response.status == 200
    data = response.json()
    assert data['full_name'] == 'Test User'
    assert data['contact'] == 'test@example.com'
    assert data['photo'] == 'photo_url'


async def test_get_profile_not_found(service_client):
    response = await service_client.get('/get-profile?id=999')
    assert response.status == 400
    data = response.json()
    assert data['message'] == 'User not found.'
