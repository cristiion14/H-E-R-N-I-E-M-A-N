using UnityEngine;
using System.Collections;

public class Pausegames : MonoBehaviour {


	
	// Update is called once per frame
	void Update () {
		if (Input.GetKeyDown (KeyCode.Escape)) {
				if(Time.timeScale == 1)
				Time.timeScale = 0;
			else
				Time.timeScale = 1;

				}
	}
}
