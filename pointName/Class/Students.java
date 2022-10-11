package Class;

import java.util.ArrayList;

/**
 * @author 林子键
 * @version 1.0
 */
public class Students {
    private ArrayList<Student> students = new ArrayList<Student>();


    public ArrayList<Student> getStudents() {
        return students;
    }

    public void setStudents(ArrayList<Student> students) {
        this.students = students;
    }

    public void addStudents(Student student){
        ArrayList<Student> students1 = getStudents();
        students1.add(student);
        this.setStudents(students1);
    }
    //生成学生花名册(所有学生)
    public void initStudents(int num) {
        for (int i = 0; i < num; i++) {
            Student student = new Student();
            student.init();
            if (i < 10)
                student.setNo("03200400" + i);
            else if (i < 100)
                student.setNo("0320040" + i);
            else if (i < 1000)
                student.setNo("032004" + i);
            students.add(student);
        }
    }



    public Student findStudent(String No){

        for(int i = 0;i < getStudents().size(); ++i) {
            if (No.equals(getStudents().get(i).getNo())){
                return getStudents().get(i);
            }
        }
        return null;
    }

}
