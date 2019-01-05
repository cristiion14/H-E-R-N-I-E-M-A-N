using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class _GM : MonoBehaviour {

    private GameObject ground;
  
	// Use this for initialization
	void Start ()
    {
        //                                                      De terminat instantierea mapei
        /*
        ground = GameObject.Find("Ground");
        for (int y = 0; y < 5; y++)
        {
            for (int x = 0; x < 5; x++)
            {
                Instantiate(ground, new Vector3(0, 0, x), Quaternion.identity);               
            }
        }
        */
    }
	
	// Update is called once per frame
	void Update ()
    {
		
	}
}
