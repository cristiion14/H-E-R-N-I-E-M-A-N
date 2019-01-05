using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ImageRotate : MonoBehaviour {


	public float rotSpeed = 0f;

	void Update () {
		transform.Rotate(Vector3.forward * Time.deltaTime * rotSpeed);
	}
}

