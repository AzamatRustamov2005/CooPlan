import pytest

from testsuite.databases import pgsql

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_disjoin_event(service_client):
    join_response = await service_client.post(
        'join_event',
        headers = {'X-Ya-User-Ticket': 'asdf'},
        json = {'event_id': 2}
    )
    disjoin_response = await service_client.delete(
        'disjoin_event',
        headers = {'X-Ya-User-Ticket': 'asdf'},
        json = {'event_id': 2}
    )
    assert disjoin_response.status == 200
    disjoin_data = disjoin_response.json()
    assert disjoin_data["status"] == 'success'
