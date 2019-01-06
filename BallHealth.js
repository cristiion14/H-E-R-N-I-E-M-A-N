#pragma strict

var maxFallDistance = -10;
var gameMaster : GameMaster;

function Update ()
{
	if (transform.position.y <= maxFallDistance)
	{
	GameMaster.currentScore = 0;
		if (GameMaster.isRestarting == false)
		{
			gameMaster.RestartLevel();
		}

	}
	
	}
	
	
