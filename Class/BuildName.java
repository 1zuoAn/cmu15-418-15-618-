package Class;

import java.util.ArrayList;

public class BuildName {
    private String firstName;
    private String lastName;
    private String name;

/*public String lineFistNameString(){
    firstName = new BuildFirstName().insideFirstName();
    lastName = new BuildLastName().insideLastName();
    name = firstName + lastName;
    return name;
}*/

    /*随机生成多个人名，通过list实现*/
    public ArrayList lineFistNameString(int index){
        ArrayList lineName = new ArrayList();
        BuildFirstName buildFirstName = new BuildFirstName();
        BuildLastName buildLastName = new BuildLastName();
        for(int i =0;i < 90;i++){
            lineName.add(buildFirstName.insideFirstName()+buildLastName.insideLastName(1));
        }
        return lineName;
    }

}

