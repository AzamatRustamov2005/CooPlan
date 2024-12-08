import pytest

from testsuite.databases import pgsql

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_disjoin_event(service_client):
    join_response = await service_client.post(
        'events/2/join',
        headers = {'Authorization': 'asdf'},
        json = {}
    )
    disjoin_response = await service_client.delete(
        'events/2/disjoin',
        headers = {'Authorization': 'asdf'},
        json = {}
    )
    assert disjoin_response.status == 200
    disjoin_data = disjoin_response.json()
    assert disjoin_data == {'status': 'success'}
