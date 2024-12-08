import pytest

from testsuite.databases import pgsql


async def test_create_event(service_client):
    json = {
        'username': 'kacok50',
        'password': 'qwerty'
    }
    register_response = await service_client.post(
        '/users/register',
        json=json
    )
    assert register_response.status == 200

    login_response = await service_client.post(
        '/users/login',
        json=json
    )
    assert login_response.status == 200
    '''
    get_response1 = await service_client.get('/events')
    assert get_response1.status == 200
    assert len(get_response1.json()) == 0
    '''
    token = login_response.json()['access_token']
    headers = {"Authorization": token}
    json = {'title': 'Community Meetup', 'image_url': 'http://example.com/images/meetup.png'}
    response = await service_client.post(
        '/events',
        headers=headers,
        json=json
    )
    assert response.status == 200
    '''
    get_response2 = await service_client.get('/events')
    assert get_response2.status == 200
    assert len(get_response2.json()) == 1
    '''