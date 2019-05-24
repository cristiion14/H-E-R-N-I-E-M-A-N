using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Objects : MonoBehaviour {

    VectorMaths.BoundingCircle sphereCol;
    VectorMaths.BoundingCircle startObjCol;

    Vector3 offset = new Vector3(0f, 0f, -5f);
    public float smoothSpeed = 0.125f;
 
    bool zMov = false;
    bool isTranslating = false;
    bool isRotating = false;
    bool isScaling = false;
    bool isScaleX, isScaleY, isScaleZ = false;
    bool isCollidingSphere, isCollidingCube, isCollidingCyl = false;
    bool isColliding;
    bool allAxisScale, allAxisRot = false;
    bool sameX, sameY, sameZ = false;
    bool sameRotX, sameRotY, sameRotZ = false;
    bool firstObjDone = false, secondObjDone = false, thirdObjDone = false;
    bool isFirstObj = true, isSecondObj = false, isThird = false;
    bool xCameraB = true, yCameraB = false;
    public static bool gameIsPaused = false;



    public GameObject pauseMenuUI;

    public GameObject targetSphere;
    public GameObject targetCube;
    public GameObject targetCylinder;
    public GameObject xCamera, yCamera;
    public GameObject sphere;
    public GameObject cube;
    public GameObject cylinder;

    public GameObject GameOverText;

    public GameObject HaveToMatchScale;
    public GameObject HaveToMatchRotation;
    public GameObject HaveToMatchPosition;

    Vector3[] cubeSpaceVert;
    Vector3[] sphereSpaceVert;
    Vector3[] cylinderSpaceVert;
    public float speed = 5f;
    public float rotSpeed = 25f;
    public float scaleSpeed = 2f;
    private GameObject currentTarget;
    private GameObject startObj;

    float rotationX, rotationY, rotationZ;
    float scaleX, scaleY, scaleZ;

    public float speedH = 5.0f;
    public float speedV = 5.0f;

    private float yaw = 0.0f;
    private float pitch = 0.0f;

    public float speed2H = 2.0f;
    public float speed2V = 2.0f;

    private float yaw2 = 0.0f;
    private float pitch2 = 0.0f;
    // Use this for initialization
    void Start () {

        //enable the task text;
        HaveToMatchScale = GameObject.Find("HaveToMatchScale");
        HaveToMatchScale.SetActive(true);

        HaveToMatchPosition = GameObject.Find("HaveToMatchPosition");
        HaveToMatchPosition.SetActive(false);

        HaveToMatchRotation = GameObject.Find("HaveToMatchRotation");
        HaveToMatchRotation.SetActive(false);

        //disable game over text
        GameOverText = GameObject.Find("GameOver");
        GameOverText.SetActive(false);

        //find each game object and set the first target and start obj active
        targetCube = GameObject.Find("dulapTarget");
        targetCube.SetActive(false);

        targetCylinder = GameObject.Find("tvTarget");
        targetCylinder.SetActive(false);

        cube = GameObject.Find("dulap");
        cube.SetActive(false);

        cylinder = GameObject.Find("tv");
        cylinder.SetActive(false);

        sphere = GameObject.Find("canapea");
        targetSphere = GameObject.Find("canapeaTarget");

        currentTarget = targetSphere;
        startObj = sphere;

        //assign the cameras
        xCamera = GameObject.FindGameObjectWithTag("Camera");
        xCamera.transform.position = new Vector3(-13.42f, 5.2f, -22.18f);

        yCamera = GameObject.Find("yCamera");
        yCamera.SetActive(false);

        //set the start position/rotation/scale of the target objects
        targetCube.GetComponent<MyTransform>().position = new Vector3(-1.6f, 0f, -6.1f);
        targetCube.GetComponent<MyTransform>().rotation = new Vector3(0, -1.5f, 0);
        targetCube.GetComponent<MyTransform>().scale = new Vector3(1,1,1);

        targetCylinder.GetComponent<MyTransform>().position = new Vector3(-8.533302f, 4.91029f, -22.6155f);
        targetCylinder.GetComponent<MyTransform>().rotation = new Vector3(0,0,0);
        targetCylinder.GetComponent<MyTransform>().scale = new Vector3(3, 3, 3);

        targetSphere.GetComponent<MyTransform>().position = new Vector3(-15.5f, 0f, 12.5f);
        targetSphere.GetComponent<MyTransform>().rotation = new Vector3(0f, 3.1f, 0f);
        targetSphere.GetComponent<MyTransform>().scale = new Vector3(1,1,1);

        startObj.GetComponent<MyTransform>().position = new Vector3(Random.Range(-22f, -4f), Random.Range(0f, 2f), Random.Range(-11f, -3f));
        startObj.GetComponent<MyTransform>().rotation = new Vector3(Random.Range(-2f,2f),Random.Range(-2f,2f),Random.Range( -2f, 2f));
        startObj.GetComponent<MyTransform>().scale = new Vector3(Random.Range(0.2f, 1.5f),Random.Range(0.2f, 1.5f),Random.Range(0.2f, 1.5f));

        rotationX = currentTarget.GetComponent<MyTransform>().rotation.x;
        rotationY = currentTarget.GetComponent<MyTransform>().rotation.y;
        rotationZ = currentTarget.GetComponent<MyTransform>().rotation.z;

        scaleX = currentTarget.GetComponent<MyTransform>().scale.x;
        scaleY = currentTarget.GetComponent<MyTransform>().scale.y;
        scaleZ = currentTarget.GetComponent<MyTransform>().scale.z;

        //define the colliders
        sphereCol = new VectorMaths.BoundingCircle(currentTarget.GetComponent<MyTransform>().position, 0.5f);
        startObjCol = new VectorMaths.BoundingCircle(startObj.GetComponent<MyTransform>().position, 0.5f);

      //  sphereCol1 = new VectorMaths.BoundingCircle(sphere.GetComponent<MyTransform>().position, 0.5f);
    }
    
	// Update is called once per frame
	void Update () {

        //update the rotation and scale of the objects
        rotationX = currentTarget.GetComponent<MyTransform>().rotation.x;
        rotationY = currentTarget.GetComponent<MyTransform>().rotation.y;
        rotationZ = currentTarget.GetComponent<MyTransform>().rotation.z;

        scaleX = currentTarget.GetComponent<MyTransform>().scale.x;
        scaleY = currentTarget.GetComponent<MyTransform>().scale.y;
        scaleZ = currentTarget.GetComponent<MyTransform>().scale.z;

        //call the functions
        movement();
        zMovement();
        rotation();
        scailing();

         Collision();
         CheckRot();
         CheckScale();
         CheckAllTransform1();
        CheckAllTransform2();
        CheckAllTransform3();

        //if yCamera button was pressed, enable the camera
        if(yCameraB)
        {
            updateCamera2();
        }

        //pause/resume game
        if(Input.GetKeyDown(KeyCode.Escape))
        {
            if(gameIsPaused)
            {
                ResumeGame();
            }
            else
            {
                PauseGame();
            }
        }

        //quick matching shortcuts for testing
        if (Input.GetKey(KeyCode.K))
        {
            startObj.GetComponent<MyTransform>().scale = currentTarget.GetComponent<MyTransform>().scale;
        }
        if(Input.GetKey(KeyCode.Space))
        {
            startObj.GetComponent<MyTransform>().position = currentTarget.GetComponent<MyTransform>().position;
        }
        if(Input.GetKey(KeyCode.R))
        {
            startObj.GetComponent<MyTransform>().rotation = currentTarget.GetComponent<MyTransform>().rotation;
        }

        //reset position
        if(Input.GetKey(KeyCode.T))
        {
            startObj.GetComponent<MyTransform>().position = new Vector3(Random.Range(-23f, -4f), Random.Range(0f, 2f), Random.Range(-11f, -3f));
            startObj.GetComponent<MyTransform>().rotation = new Vector3(Random.Range(-2f, 2f), Random.Range(-2f, 2f), Random.Range(-2f, 2f));
            startObj.GetComponent<MyTransform>().scale = new Vector3(Random.Range(0.2f, 1.5f), Random.Range(0.2f, 1.5f), Random.Range(0.2f, 1.5f));
        }
    }
    
    void PauseGame()
    {
        pauseMenuUI.SetActive(true);
        Time.timeScale = 0f; // speed that time is passing
        gameIsPaused = true;
    }
    void ResumeGame()
    {
        pauseMenuUI.SetActive(false);
        Time.timeScale = 1f;
        gameIsPaused = false;
    }
    
    void FixedUpdate()
    {
        //xCamera follower
       float tempPos = xCamera.transform.rotation.y;
        Vector3 desiredPosition = startObj.GetComponent<MyTransform>().position + offset;
        Vector3 smoothedPos = VectorMaths.LinearInterpolateValue(xCamera.transform.position, desiredPosition, smoothSpeed);
        xCamera.transform.position = smoothedPos;

        //zoom in and out by lowering/growing the offset
        if(Input.GetKey(KeyCode.Minus))
        {
            offset.z -= .5f;
        }
        else if(Input.GetKey(KeyCode.Equals))
        {
            offset.z += .5f;
        }

       
        
    }
    void updateCamera2()
    {
        //yCamera rot   
        if (!gameIsPaused)
        {
            yaw2 += speed2H * Input.GetAxis("Mouse X");
            pitch2 -= speed2V * Input.GetAxis("Mouse Y");
            //Vector3 newPos = VectorMaths.EulerAngletoDir(new Vector3(pitch2, yaw2, 0));
            //yCamera.transform.eulerAngles = newPos;
       //  yCamera.GetComponent<MyTransform>().rotation =  VectorMaths.EulerAngletoDir (new Vector3(pitch2, yaw2, 0.0f));
            yCamera.transform.eulerAngles = new Vector3(pitch2, yaw2, 0.0f);
        }
        
    }
    
    void CheckAllTransform1()
    {
        //check to see it is the first object
        if (isFirstObj)
        {
            //check for each transformation starting with rotation, if no transformation is matched display the first task 
            if(!allAxisRot)
            {
                //HaveToMatchScale.SetActive(true);
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.white;
                HaveToMatchScale.SetActive(false);
                HaveToMatchRotation.SetActive(true);
                HaveToMatchPosition.SetActive(false);
            }
            //if the first transform is matched, color the object blue and update the task
            else if (allAxisRot)
            {
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.cyan;
                HaveToMatchRotation.SetActive(false);
                  
            }
            if (!allAxisScale && allAxisRot)
            {
                HaveToMatchRotation.SetActive(false);
                HaveToMatchScale.SetActive(true);
                HaveToMatchPosition.SetActive(false);
                
            }
            //check for the next transformation match and if so update the color and the task text
            else if(allAxisScale && allAxisRot)
            {
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.red;
                HaveToMatchScale.SetActive(false);
               
            }
            if(allAxisScale && allAxisRot && !isColliding)
            {
                HaveToMatchPosition.SetActive(true);
                HaveToMatchRotation.SetActive(false);
                HaveToMatchScale.SetActive(false);
                
                
            }
            //if all the transform are done for the first object end the if statement
           else  if (allAxisScale && isColliding && allAxisRot)
            {
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.green;
                firstObjDone = true;
                isFirstObj = false;
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.white;
                HaveToMatchPosition.SetActive(false);
                
            }
        }
        //if all the transform are matched,change the start object to the next one and reset it's transformations
        if (firstObjDone)
        {
            allAxisRot = false;
            allAxisScale = false;
            isColliding = false;

            targetSphere.SetActive(false);
            sphere.SetActive(false);

            currentTarget = targetCube;
            targetCube.SetActive(true);

            cube.SetActive(true);
            startObj = cube;

            firstObjDone = false;
            isSecondObj = true;
            startObj.GetComponent<MyTransform>().position = new Vector3(Random.Range(-23f, -4f), Random.Range(0f, 2f), Random.Range(-11f, -3f));
            startObj.GetComponent<MyTransform>().rotation = new Vector3(Random.Range(-2f, 2f), Random.Range(-2f, 2f), Random.Range(-2f, 2f));
            startObj.GetComponent<MyTransform>().scale = new Vector3(Random.Range(0.2f, 1.5f), Random.Range(0.2f, 1.5f), Random.Range(0.2f, 1.5f));
            CheckAllTransform2();
          
        }
    
    }
    //analog for the second and third checking functions
    void CheckAllTransform2()
    {
        if (isSecondObj)
        {
            if (!allAxisRot)
            {
                //HaveToMatchScale.SetActive(true);
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.white;
                HaveToMatchScale.SetActive(false);
                HaveToMatchRotation.SetActive(true);
                HaveToMatchPosition.SetActive(false);
            }
            else if (allAxisRot)
            {
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.cyan;
                HaveToMatchRotation.SetActive(false);

            }

            if (!allAxisScale && allAxisRot)
            {
                HaveToMatchRotation.SetActive(false);
                HaveToMatchScale.SetActive(true);
                HaveToMatchPosition.SetActive(false);

            }
            else if (allAxisScale && allAxisRot)
            {
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.red;
                HaveToMatchScale.SetActive(false);

            }
            if (allAxisScale && allAxisRot && !isColliding)
            {
                HaveToMatchPosition.SetActive(true);
                HaveToMatchRotation.SetActive(false);
                HaveToMatchScale.SetActive(false);


            }
            else if (allAxisScale && isColliding && allAxisRot)
            {
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.green;
                secondObjDone = true;
                isSecondObj = false;
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.white;
                HaveToMatchPosition.SetActive(false);

            }

        }
        if (secondObjDone)
        {
            allAxisRot = false;
            allAxisScale = false;
            isColliding = false;

            targetCube.SetActive(false);
            cube.SetActive(false);

            currentTarget = targetCylinder;
            targetCylinder.SetActive(true);

            cylinder.SetActive(true);
            startObj = cylinder;

            secondObjDone = false;
            isThird = true;
            startObj.GetComponent<MyTransform>().position = new Vector3(Random.Range(-23f, -4f), Random.Range(0f, 2f), Random.Range(-11f, -3f));
            startObj.GetComponent<MyTransform>().rotation = new Vector3(Random.Range(-2f, 2f), Random.Range(-2f, 2f), Random.Range(-2f, 2f));
            startObj.GetComponent<MyTransform>().scale = new Vector3(Random.Range(0.2f, 1.5f), Random.Range(0.2f, 1.5f), Random.Range(0.2f, 1.5f));
            CheckAllTransform3();
        }
       
    }
    void CheckAllTransform3()
    {
        if (isThird)
        {
            if (!allAxisRot)
            {
                //HaveToMatchScale.SetActive(true);
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.white;
                HaveToMatchScale.SetActive(false);
                HaveToMatchRotation.SetActive(true);
                HaveToMatchPosition.SetActive(false);
            }
            else if (allAxisRot)
            {
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.cyan;
                HaveToMatchRotation.SetActive(false);

            }

            if (!allAxisScale && allAxisRot)
            {
                HaveToMatchRotation.SetActive(false);
                HaveToMatchScale.SetActive(true);
                HaveToMatchPosition.SetActive(false);

            }
            else if (allAxisScale && allAxisRot)
            {
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.red;
                HaveToMatchScale.SetActive(false);

            }
            if (allAxisScale && allAxisRot && !isColliding)
            {
                HaveToMatchPosition.SetActive(true);
                HaveToMatchRotation.SetActive(false);
                HaveToMatchScale.SetActive(false);


            }
            else if (allAxisScale && isColliding && allAxisRot)
            {
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.green;
               // firstObjDone = true;
               // secondObjDone = true;
                thirdObjDone = true;
                isThird = false;
                currentTarget.GetComponent<MeshRenderer>().material.color = Color.white;
                HaveToMatchPosition.SetActive(false);

            }
        }
        /*
        Debug.Log("suntem la primul obiect? " + isFirstObj);
        Debug.Log("suntem la al doilea ob? " + isSecondObj);
        Debug.Log("suntem la al 3 lea ob?"+isThird);
        Debug.Log("A terminat treaba ob 3? " + thirdObjDone);
        */

        //When every task is done Set active the Game over text and close the app
        if(thirdObjDone)
        {
            allAxisRot = false;
            allAxisScale = false;
            isColliding = false;

            GameOverText.SetActive(true);
            Application.Quit();
            Debug.Log("!!!GAME OVER!!!\n");
          //  thirdObjDone = false;
        }
    }

    void Collision()
    {
        //set the center point of each circles
        sphereCol.CentrePoint = currentTarget.GetComponent<MyTransform>().position;
        startObjCol.CentrePoint = startObj.GetComponent<MyTransform>().position;
        
        if (startObjCol.Intersects(sphereCol))
        {
            Debug.Log("Collision!\n");
           // targetSphere.GetComponent<MeshRenderer>().material.color = Color.green;
            isColliding = true;
        }
        else
        {
            isColliding = false;
        }

    }
    void CheckScale()
    {
        //variables to store the start object scale
        float tempScaleX = startObj.GetComponent<MyTransform>().scale.x;
        float tempScaleY = startObj.GetComponent<MyTransform>().scale.y;
        float tempScaleZ = startObj.GetComponent<MyTransform>().scale.z;

        //check the scale on each axis having a 0.35 margin of error
        if (tempScaleX - scaleX >= -0.35f && tempScaleX - scaleX < 0.35f)
        {
            sameX = true;
            //Debug.Log("Same Scale on X");
        }
        else
        {
            sameX = false;
        }

        if (tempScaleY - scaleY >= -0.35f && tempScaleY - scaleY < 0.35f)
        {
            sameY = true;
        }
        else
        {
            sameY = false;
        }

        if (tempScaleZ - scaleZ >= -0.35f && tempScaleZ - scaleZ < 0.35f)
        {
            sameZ = true;
        }
        else
        {
            sameZ = false;
        }

        if (sameX && sameY && sameZ)
        {
            //targetSphere.GetComponent<MeshRenderer>().material.color = Color.blue;
            allAxisScale = true;
            
        }
        else
        {
            allAxisScale = false;
        }

      //  Debug.Log("sunt toate la fel? "+allAxisScale);
    }
    void CheckRot()
    {
        //var to store the rotation of the start obj
        float rotX = startObj.GetComponent<MyTransform>().rotation.x;
        float rotY = startObj.GetComponent<MyTransform>().rotation.y;
        float rotZ = startObj.GetComponent<MyTransform>().rotation.z;

        //check rotation on each axis with a 0.5 margin of error
        if (rotX - rotationX >= -0.5f && rotX - rotationX <= 0.5f)
        {
            sameRotX = true;
        }
        else
        {
            sameRotX = false;
        }
        if (rotY - rotationY >= -0.5f && rotY - rotationY <= 0.5f)
        {
            sameRotY = true;
        }
        else
            sameRotY = false;

        if (rotZ - rotationZ >= -0.5f && rotZ - rotationZ <= 0.5f)
        {
            sameRotZ = true;
        }
        else
            sameRotZ = false;

        if (sameRotX && sameRotY && sameRotZ)
        {
            // targetSphere.GetComponent<MeshRenderer>().material.color = Color.yellow;
            allAxisRot = true;
        }
        else
        {
            allAxisRot = false;
        }
    }
   public void xMov()
    {
        //Function to enable the x movement button
        isRotating = false;
        isScaleX = false;
        isScaleY = false;
        isScaleZ = false; 
        zMov = false;
        if (isTranslating == false)
            isTranslating = true;
        else
            isTranslating = false;
        Debug.Log(isTranslating);
    }
   public void xCameraButton()
    {
        //function to enable the xCamera button
        yCameraB = false;
        yCamera.SetActive(false);
        if (xCameraB == false)
        {
            xCameraB = true;
            xCamera.SetActive(true);
        }
        else
            xCameraB = false;
    }
   public void yCameraButton()
    {
        //function to enable the yCamera button
        xCameraB = false;
        xCamera.SetActive(false);
        if (yCameraB == false)
        {
            yCameraB = true;
            yCamera.SetActive(true);
        }
        else
            yCameraB = false;
    }
    public void zButton()
    {
        //enable the z movement button
        isRotating = false;
        isScaleX = false;
        isScaleY = false;
        isScaleZ = false;
        isTranslating = false;
        if (zMov == false)
            zMov = true;
        else
            zMov = false;
        Debug.Log(zMov);
    }
    void zMovement()
    {
        //perform the movement on z axis
        if(zMov&&!isTranslating)
        {
            if (Input.GetKey(KeyCode.UpArrow))
            {
                startObj.GetComponent<MyTransform>().position += Vector3.forward * Time.deltaTime * speed;
     
            }
            else if(Input.GetKey(KeyCode.DownArrow))
            {
                startObj.GetComponent<MyTransform>().position += Vector3.back * Time.deltaTime * speed;
           
            }
        }
    }
    public void rotButton()
    {
        //enable the rotation button
        isScaleX = false;
        isScaleY = false;
        isScaleZ = false;
        isTranslating = false;
        zMov = false;
        if (isRotating == false)
            isRotating = true;
        else
            isRotating = false;
        Debug.Log(isRotating);
    }
    //next three functions enables the scaling on each axis
    public void isScaleXButton()
    {
        isTranslating = false;
        isRotating = false;
        zMov = false;
        isScaleY = false;
        isScaleZ = false;
        if (isScaleX==false)
            isScaleX = true;
        else
            isScaleX = false;
        Debug.Log(isScaleX);
    }
    public void isScaleYButton()
    {
        isTranslating = false;
        zMov = false;
        isRotating = false;
        isScaleX = false;
        isScaleZ = false;
        if (isScaleY==false)
            isScaleY = true;
        else
            isScaleY = false;
        Debug.Log(isScaleY);
    }
    public void isScaleZButton()
    {
        isTranslating = false;
        zMov = false;
        isRotating = false;
        isScaleY = false;
        isScaleX = false;
        if (isScaleZ==false)
            isScaleZ = true;
        else
            isScaleZ = false;
        Debug.Log(isScaleZ);
    }

    void scailing()
    {
        //function which scales an object up by pressing s and down by pressing c
            if (isScaleX)
            {
                if (Input.GetKey(KeyCode.S))
                {
                    startObj.GetComponent<MyTransform>().scale.x += scaleSpeed * Time.deltaTime;
                }
                else if(Input.GetKey(KeyCode.C))
                {
                    startObj.GetComponent<MyTransform>().scale.x -= scaleSpeed * Time.deltaTime;
                }
            }
            if(isScaleY)
            {
                if (Input.GetKey(KeyCode.S))
                {
                    startObj.GetComponent<MyTransform>().scale.y += scaleSpeed * Time.deltaTime;
                }
                else if (Input.GetKey(KeyCode.C))
                {
                    startObj.GetComponent<MyTransform>().scale.y -= scaleSpeed * Time.deltaTime;
                }
            }
             if (isScaleZ)
            {
                if (Input.GetKey(KeyCode.S))
                {
                    startObj.GetComponent<MyTransform>().scale.z += scaleSpeed * Time.deltaTime;
                }
            else if (Input.GetKey(KeyCode.C))
                {
                    startObj.GetComponent<MyTransform>().scale.z -= scaleSpeed * Time.deltaTime;
                }
            }
        }
    void rotation()
    {
        //function which updates the rotation of the object, resetting the values for easy to match reasons
        if(isRotating)
        {
            if (Input.GetKey(KeyCode.X)&&Input.GetKey(KeyCode.RightArrow))
            {
                startObj.GetComponent<MyTransform>().rotation.x += Time.deltaTime * rotSpeed;
                if(startObj.GetComponent<MyTransform>().rotation.x >=3.6f || startObj.GetComponent<MyTransform>().rotation.x <=-1.8f)
                {
                    startObj.GetComponent<MyTransform>().rotation.x = 0;
                }
                Debug.Log(startObj.GetComponent<MyTransform>().rotation.x);
            }
            else if(Input.GetKey(KeyCode.X) && Input.GetKey(KeyCode.LeftArrow))
            {
                startObj.GetComponent<MyTransform>().rotation.x -= Time.deltaTime * rotSpeed;
                if (startObj.GetComponent<MyTransform>().rotation.x >= 3.6f || startObj.GetComponent<MyTransform>().rotation.x <= -1.8f)
                {
                    startObj.GetComponent<MyTransform>().rotation.x = 0;
                }
                Debug.Log(startObj.GetComponent<MyTransform>().rotation.x);
            }
             if (Input.GetKey(KeyCode.Y) && Input.GetKey(KeyCode.RightArrow))
            {
                startObj.GetComponent<MyTransform>().rotation.y += Time.deltaTime * rotSpeed;
                if (startObj.GetComponent<MyTransform>().rotation.y >= 3.6f || startObj.GetComponent<MyTransform>().rotation.y <= -1.8f)
                    startObj.GetComponent<MyTransform>().rotation.y = 0;
            }
            else if (Input.GetKey(KeyCode.Y) && Input.GetKey(KeyCode.LeftArrow))
            {
                startObj.GetComponent<MyTransform>().rotation.y -= Time.deltaTime * rotSpeed;
                if (startObj.GetComponent<MyTransform>().rotation.y >= 3.6f || startObj.GetComponent<MyTransform>().rotation.y <= -1.8f)
                    startObj.GetComponent<MyTransform>().rotation.y = 0;
                //  Debug.Log(startObj.GetComponent<MyTransform>().rotation.x);
            }
             if (Input.GetKey(KeyCode.Z)&&Input.GetKey(KeyCode.RightArrow))
                 {
                     startObj.GetComponent<MyTransform>().rotation.z += Time.deltaTime * rotSpeed;
                if (startObj.GetComponent<MyTransform>().rotation.z >= 3.6f || startObj.GetComponent<MyTransform>().rotation.z <= -1.8f)
                    startObj.GetComponent<MyTransform>().rotation.z = 0;
            }
            else if (Input.GetKey(KeyCode.Z) && Input.GetKey(KeyCode.LeftArrow))
            {
                startObj.GetComponent<MyTransform>().rotation.z -= Time.deltaTime * rotSpeed;
                if (startObj.GetComponent<MyTransform>().rotation.z >= 3.6f || startObj.GetComponent<MyTransform>().rotation.z <= -1.8f)
                    startObj.GetComponent<MyTransform>().rotation.z = 0;
                //  Debug.Log(startObj.GetComponent<MyTransform>().rotation.x);
            }
        }
    }
    void movement()
    {
        //function which updates the position of the player
        //if the xMovement button has been pressed 
        if (isTranslating&&!zMov)
        {
            if (Input.GetKey(KeyCode.LeftArrow))
            {

                //gb.transform.position += Vector3.left * Time.deltaTime * speed;
                startObj.GetComponent<MyTransform>().position += Vector3.left * Time.deltaTime * speed;


            }
            else if (Input.GetKey(KeyCode.RightArrow))
            {

                startObj.GetComponent<MyTransform>().position += Vector3.right * Time.deltaTime * speed;

            }
            if (Input.GetKey(KeyCode.UpArrow))
            {

                startObj.GetComponent<MyTransform>().position += Vector3.up * Time.deltaTime * speed;


            }
            else if (Input.GetKey(KeyCode.DownArrow))
            {
                startObj.GetComponent<MyTransform>().position += Vector3.down * Time.deltaTime * speed;

            }
        }
    }
}


