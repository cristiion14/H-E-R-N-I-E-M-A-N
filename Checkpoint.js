#pragma strict

static var ReachedPoint : Vector3 = Vector3 (0, 1.5, 0);

function OnTriggerEnter (col : Collider)
{
	if (col.tag == "Player")
	{
		if (transform.position.x > ReachedPoint.x) {
			ReachedPoint = transform.position;
			GameMaster.currentScore = GameMaster.currentScore + 0;
		}
	}
}