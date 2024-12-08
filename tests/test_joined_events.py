import pytest

from testsuite.databases import pgsql

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_no_token(service_client):
    headers = {}
    response = await service_client.get('/my_events', headers = headers, json = {})
    assert response.status == 401
    assert response.json() == {'message': 'Invalid or missing access token'}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_see_joined_events1(service_client):
    token = 'session_token_participant1'
    headers = {"Authorization": token}
    data = {}
    response = await service_client.get('/my_events', headers = headers, json = data)
    assert response.status == 200

    json_response = response.json()

    assert isinstance(json_response, list)

    expected_data = [
        {"event_id": 4, "event_name": "Charity Run"}
    ]
    assert len(json_response) == len(expected_data)
    for expected, actual in zip(expected_data, json_response):
        assert expected == actual

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_see_joined_events2(service_client):
    token = 'session_token_participant2'
    headers = {"Authorization": token}
    data = {}
    response = await service_client.get('/my_events', headers = headers, json = data)
    assert response.status == 200

    json_response = response.json()

    assert isinstance(json_response, list)

    expected_data = []
    assert len(json_response) == len(expected_data)
    for expected, actual in zip(expected_data, json_response):
        assert expected == actual