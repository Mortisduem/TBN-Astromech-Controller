void animations(){

  //Padreo Padreo

  if (Animation_Pedro > 0){
    Animation_Pedro--;
    Serial.println(Animation_Pedro);
    
    if (Animation_Pedro < 950){
      triggerI2C(dome_lights,0);
      triggerI2C(body_lights,0);
      body.setPWM(8,0, LeftDoorClose);
      body.setPWM(1,0, RightDoorClose);
  		body.setPWM(4,0,UtlityLowwerClose);
		body.setPWM(5,0,UtlityUpperClose);
      Animation_Pedro = 0;
    }
    else if (Animation_Pedro < 1450){
      if((Animation_Pedro + 1) % 6 == 0){
        triggerI2C(body_lights,10);
      }
      else if((Animation_Pedro +1) % 6 == 3){
        triggerI2C(body_lights,9);
      }
      triggerI2C(dome_lights,2 + ((Animation_Pedro/4) % 6));
    }
    else if (Animation_Pedro < 1510){
      body.setPWM(8,0, LeftDoorClose);
      body.setPWM(1,0, RightDoorClose);
  
				//body.setPWM(4,0,UtlityLowwerOpen);
				//body.setPWM(5,0,UtlityUpperOpen);
    }
    else if (Animation_Pedro < 1525){
      
  body.setPWM(2,0,GripperArmIn);
  body.setPWM(9,0,InterfaceArmIn);
    }
    else if (Animation_Pedro < 1810){
      if((Animation_Pedro + 1) % 6 == 0){
        triggerI2C(body_lights,10);
        //body.setPWM(8,0, LeftDoorOpen);
      //body.setPWM(1,0, RightDoorClose);
      }
      else if((Animation_Pedro +1) % 6 == 3){
        triggerI2C(body_lights,9);
      }
      if ((Animation_Pedro + 1) % 6 == 0){
        body.setPWM(2,0,GripperArmOut);
        body.setPWM(9,0,InterfaceArmOut);
      }
      else 
      if ((Animation_Pedro + 1) % 6 == 3){
        body.setPWM(2,0,GripperArmOut-200);
        body.setPWM(9,0,InterfaceArmOut+100);
      }
      triggerI2C(dome_lights,2 + ((Animation_Pedro/4) % 6));
    }
    else if (Animation_Pedro < 1820) {
      body.setPWM(2,0,GripperArmOut);
      body.setPWM(9,0,InterfaceArmOut);
    }
    else if (Animation_Pedro < 1830) {      
      body.setPWM(8,0, LeftDoorOpen);
      body.setPWM(1,0, RightDoorOpen);
    }
    else if (Animation_Pedro < 1950){
      triggerI2C(body_lights,0);
    }
    else if (Animation_Pedro < 2075){
        triggerI2C(body_lights,8);
        triggerI2C(dome_lights,0);
    }
    else if (Animation_Pedro < 2100) {      
      body.setPWM(8,0, LeftDoorClose);
      body.setPWM(1,0, RightDoorClose);
    }
    else if(Animation_Pedro < 2125){
        body.setPWM(2,0,GripperArmIn);
        body.setPWM(9,0,InterfaceArmIn);
    }
    else if (Animation_Pedro < 1830) {      
      body.setPWM(8,0, LeftDoorOpen);
      body.setPWM(1,0, RightDoorOpen);
    }
    else if (Animation_Pedro < 2330){
      if((Animation_Pedro + 1) % 6 == 0){
        triggerI2C(body_lights,10);
      }
      else if((Animation_Pedro +1) % 6 == 3){
        triggerI2C(body_lights,9);
      }
      if ((Animation_Pedro + 1) % 6 == 0){
        body.setPWM(2,0,GripperArmOut);
        body.setPWM(9,0,InterfaceArmOut);
      }
      else 
      if ((Animation_Pedro + 1) % 6 == 3){
        body.setPWM(2,0,GripperArmOut-200);
        body.setPWM(9,0,InterfaceArmOut+100);
      }
      triggerI2C(dome_lights,2 + ((Animation_Pedro/4) % 6));
      body.setPWM(4,0,UtlityLowwerClose);
      body.setPWM(5,0,UtlityUpperClose);
    }
    else if (Animation_Pedro < 2340) {      
      body.setPWM(8,0, LeftDoorOpen);
      body.setPWM(1,0, RightDoorOpen);
    }
    else if (Animation_Pedro < 2595){
      triggerI2C(dome_lights,9);
      triggerI2C(body_lights,9);
      body.setPWM(4,0,(UtlityLowwerOpen+UtlityLowwerClose)/2);
    }
    else if (Animation_Pedro < 2610){
      triggerI2C(body_lights,8);
      body.setPWM(4,0,UtlityLowwerClose);
      body.setPWM(5,0,UtlityUpperClose);
    }
    else if (Animation_Pedro < 2745){
      triggerI2C(dome_lights,10);
      triggerI2C(body_lights,10);
      //body.setPWM(4,0,UtlityLowwerOpen);
			body.setPWM(5,0,(UtlityUpperOpen+UtlityUpperClose)/2);
    }
    else if (Animation_Pedro < 2760){
      triggerI2C(body_lights,8);
    }
    else if (Animation_Pedro < 2875){
      if((Animation_Pedro + 1) % 6 == 0){
        triggerI2C(body_lights,10);
      }
      else if((Animation_Pedro +1) % 6 == 3){
        triggerI2C(body_lights,9);
      }
      triggerI2C(dome_lights,2 + ((Animation_Pedro/4) % 6));
    }
    else if (Animation_Pedro < 3000){
      triggerI2C(dome_lights,8);
      triggerI2C(body_lights,8);
    }
  }
}


