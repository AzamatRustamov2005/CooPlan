import pytest

from testsuite.databases import pgsql

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_join_event(service_client):
    
    join_response = await service_client.post(
        'join_event',
        headers = {'X-Ya-User-Ticket': 'asdf'},
        json = {'event_id': 2}
    )
    assert join_response.status == 200
    join_data = join_response.json()
    assert join_data["status"] == 'success'
