import pytest

from testsuite.databases import pgsql

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_no_token(service_client):
    token = 'session_token_participant1'
    headers = {}
    data = {}
    response = await service_client.get('/requests_for_organizator', headers = headers, json = data)
    assert response.status == 401

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_see_requests(service_client):
    token = 'session_token_organizer1'
    headers = {"X-Ya-User-Ticket": token}
    data = {}
    response = await service_client.get('/requests_for_organizator', headers = headers, json = data)
    assert response.status == 200

    json_response = response.json()

    assert isinstance(json_response, list)

    expected_data = [
        {"event_id": 1, "event_name": "Tech Meetup", "user_id": 3, "user_username": "participant1"},
        {"event_id": 1, "event_name": "Tech Meetup", "user_id": 4, "user_username": "participant2"}
    ]
    assert len(json_response) == len(expected_data)
    for expected, actual in zip(expected_data, json_response):
        assert expected == actual
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_see_requests_1(service_client):
    token = 'session_token_organizer2'
    headers = {"X-Ya-User-Ticket": token}
    data = {}
    response = await service_client.get('/requests_for_organizator', headers = headers, json = data)
    assert response.status == 200

    json_response = response.json()

    assert isinstance(json_response, list)

    expected_data = [
        {"event_id": 2, "event_name": "Charity Run", "user_id": 4, "user_username": "participant2"},
    ]
    assert len(json_response) == len(expected_data)
    for expected, actual in zip(expected_data, json_response):
        assert expected == actual