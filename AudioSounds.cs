using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AudioSounds : MonoBehaviour {

	
	// Update is called once per frame
	void Update ()
    {
		if(Input.GetKeyDown(KeyCode.E))
        {
            AudioSource shoot = GetComponent<AudioSource>();
            shoot.Play();
        }
	}
}
