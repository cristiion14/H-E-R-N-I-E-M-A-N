using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AnimController : MonoBehaviour {
    public KeyCode MyKey;
    public string MyTrigger;

    void Update()
    {
        if (Input.GetKey(MyKey))
        {
            GetComponent<Animator>().SetTrigger(MyTrigger);
        }
    }

}
