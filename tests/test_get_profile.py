import pytest


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_profile(service_client):
    response = await service_client.get('/get-profile/123')
    assert response.status == 200
    data = response.json()
    assert data['username'] == 'test'
    assert data['contact'] == 'test@example.com'
    assert data['image'] == 'photo_url'


async def test_get_profile_not_found(service_client):
    response = await service_client.get('/get-profile/999')
    assert response.status == 400
    data = response.json()
    assert data['message'] == 'User not found.'
