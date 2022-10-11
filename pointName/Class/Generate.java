package Class;

import java.sql.Connection;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;

/**
 * @author 林子键
 * @version 1.0
 */
public class Generate {
    //生成学生总表
    public static Students generateStudents(int num) {
        Students students = new Students();
        students.initStudents(num);
        return students;

    }

    //生成课程总表
    public static Lessons generateLessons(int num, Students students) {
        Lessons lessons = new Lessons();
        lessons.initCourses(num, students);
        return lessons;
    }


    public Lessons generateMis(Lessons lessons) {
        lessons.createAllMis();
        return lessons;
    }

    public void write(Lessons lessons, int cnt, Read read) {
        Connection con = read.getCon();
        for (int i = 0; i < lessons.getCourses().size(); ++i) {
            Course course = lessons.getCourses().get(i);
            for (int j = 0; j < course.getStudents().size(); j++) {
                Student student = course.getStudents().get(j);
                String No = student.getNo();
                String name = student.getName();
                double grade = student.getGrade();
                String records = student.infoRecords(course, cnt);
                read.execute(con, "REPLACE INTO RECORDS (No,name,grade,Course)" +
                            "values('" + No  + "','" + name + "'," + grade +
                            ",'" + records + "');");

            }

        }
    }
    public void pointWrite(Lessons lessons, int cnt, Read read) {
        Connection con = read.getCon();
        for (int i = 0; i < lessons.getCourses().size(); ++i) {
            Course course = lessons.getCourses().get(i);
            for (int j = 0; j < course.getStudents().size(); j++) {
                Student student = course.getStudents().get(j);
                String No = student.getNo();
                String name = student.getName();
                double grade = student.getGrade();
                String records = course.infoPointRecords(student, cnt);
                read.execute(con, "REPLACE INTO POINTRECORDS (No,name,grade,Course)" +
                            "values('" + No  + "','" + name + "'," + grade +
                            ",'" + records + "');");

            }

        }
    }

    public Double generateE(Lessons lessons){
        int goodPoint = 0;
        int sumPoint = 0;
        for(int i = 0; i < lessons.getCourses().size(); i++){
            Course course = lessons.getCourses().get(i);
            HashMap<Student,ArrayList<Integer>> hashMap = course.getPointRecords();
            for(Student key: hashMap.keySet()){
                ArrayList<Integer> arrayList = hashMap.get(key);
                for (Integer integer : arrayList) {
                    if (integer == 1) {
                        goodPoint++;
                        sumPoint++;
                    } else if (integer == 2) {
                        sumPoint++;
                    }
                }
            }
        }
        return 1.0*goodPoint/sumPoint;
    }





}
