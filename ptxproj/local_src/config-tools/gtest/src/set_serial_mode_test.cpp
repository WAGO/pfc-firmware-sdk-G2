//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     set_serial_mode_test.cpp
///
///  \brief    Test for configtool set_serial_mode.
///
///  \author   Jobst Wellensiek : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#include <glib.h>
#pragma GCC diagnostic pop

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <termios.h>
#include <unistd.h>

int CallExecutable(const std::string & command, gchar ** ppOut);

int CallExecutable(const std::string & command, gchar ** ppOut)
{
  gchar * standard_output = nullptr;
  gchar * standard_error = nullptr;
  gint exit_status = 0;
  GError * err = nullptr;
  int result_func = 0;
  gboolean result =
      g_spawn_command_line_sync(command.c_str(), &standard_output, &standard_error, &exit_status, &err);

  if (result == false)
  {
    std::string msg{"Could not run command '" + command +
                    "': " + (err != nullptr ? err->message : "Unknown error")};
    g_clear_error(&err);

    throw std::invalid_argument{msg};
  }
  result = g_spawn_check_exit_status(exit_status, &err);
  if (result == false)
  {
    if (err->code == G_FILE_ERROR_ACCES)
    {
      result_func = ENOENT;
    }
    else
    {
      if (err->code == G_FILE_ERROR_AGAIN)
      {
        std::cerr << "Command did not exit normally: " << err->message << std::endl;
        result_func = EAGAIN;
      }
      else
      {
        std::cerr << "Command did not exit normally: " << err->message << std::endl;
        std::cerr << "Standard output:\n";
        std::cerr << "<<<<--- start --->>>>\n";
        std::cerr << (standard_output != nullptr ? standard_output : "");
        std::cerr << "<<<<---  end  --->>>>\n";
        std::cerr << "Standard error:\n";
        std::cerr << "<<<<--- start --->>>>\n";
        std::cerr << (standard_error != nullptr ? standard_error : "");
        std::cerr << "<<<<---  end  --->>>>\n";
      }
      g_free(standard_output);
      g_free(standard_error);

      return result_func;
    }
  }

  // If we got here then the command ran and exited normally
  // FIXME: The caller should have a possibility to decide what to do with the output
  if (ppOut != nullptr)
  {
    *ppOut = standard_output;
  }

  g_free(standard_error);

  return result_func;
}

bool operator==(const termios& l, const termios& r);
bool operator==(const termios& l, const termios& r)
{
  return    (l.c_iflag == r.c_iflag)
         && (l.c_oflag == r.c_oflag)
         && (l.c_cflag == r.c_cflag)
         && (l.c_lflag == r.c_lflag)
         && (l.c_cc[0] == r.c_cc[0])
         && (l.c_cc[1] == r.c_cc[1])
         && (l.c_cc[2] == r.c_cc[2])
         && (l.c_cc[3] == r.c_cc[3])
         && (l.c_cc[4] == r.c_cc[4])
         && (l.c_cc[5] == r.c_cc[5])
         && (l.c_cc[6] == r.c_cc[6])
         && (l.c_cc[7] == r.c_cc[7]);
 }
std::ostream & operator<<(std::ostream & os, const termios & rs);
std::ostream & operator<<(std::ostream & os, const termios & rs)
{
   os << "{ " << rs.c_iflag << ", " << rs.c_oflag << ", " << rs.c_cflag << ", " << rs.c_lflag << ", [";
   bool first = true;
   for (unsigned int i= 0; i< (sizeof(rs.c_cc)/sizeof(__u32)); i++)
   {
     if (!first)
       os << ", ";
     os << rs.c_cc[i];
     first = false;
   }
   os << "]}";
   return os;
}


bool operator==(const serial_rs485& l, const serial_rs485& r);
bool operator==(const serial_rs485& l, const serial_rs485& r)
{
  return    (l.flags == r.flags)
         && (l.delay_rts_after_send == r.delay_rts_after_send)
         && (l.delay_rts_before_send == r.delay_rts_before_send)
         && (l.padding[0] == r.padding[0])
         && (l.padding[1] == r.padding[1])
         && (l.padding[2] == r.padding[2])
         && (l.padding[3] == r.padding[3])
         && (l.padding[4] == r.padding[4]);
 }
std::ostream & operator<<(std::ostream & os, const serial_rs485 & rs);
std::ostream & operator<<(std::ostream & os, const serial_rs485 & rs)
{
   os << "{ " << rs.flags << ", " << rs.delay_rts_after_send << ", " << rs.delay_rts_before_send << ",[";
   bool first = true;
   for (unsigned int i= 0; i< (sizeof(rs.padding)/sizeof(__u32)); i++)
   {
     if (!first)
       os << ", ";
     os << rs.padding[i];
     first = false;
   }
   os << "]}";
   return os;
}

#define TTY "/dev/serial"

class tty
{
  public:
    tty(std::string drv){
      fd = open(drv.c_str(), O_RDWR);
      if (fd >= 0)
      {
        if(   (ioctl(fd, TIOCGRS485, &rs485) < 0)
            || (tcgetattr(fd, &tp) < 0)) {
          std::cout << "Error: settings not readable." << std::endl;
          close(fd);
          fd = -1;
        }
      }
    }
    ~tty(){
      if (fd >= 0)
      {
        tcflush(fd, TCIOFLUSH); // Flush buffers
        if(  (ioctl(fd, TIOCSRS485, &rs485) < 0)
           ||(tcsetattr(fd, TCSANOW, &tp) < 0))
        {
          std::cout << "Error:  setting not writeable." << std::endl;
        }
        tcflush(fd, TCIOFLUSH); // Flush buffers
        close(fd);
      }
    }
    serial_rs485 getrs485()
    {
      serial_rs485 rs485conf;
      memset(&rs485conf,0,sizeof(rs485conf));
      if (fd >= 0)
      {
        if(ioctl(fd, TIOCGRS485, &rs485conf) < 0) {
           std::cout << "Error: TIOCGRS485 ioctl not supported." << std::endl;
       }
      }
      else
      {
        std::cout << "Error: file descriptor not get." << std::endl;
      }
      return rs485conf;
    }

    termios getTCSANOW()
    {
      termios tp_conf;
      memset(&tp_conf,0,sizeof(tp_conf));
      if (fd >= 0)
      {
        if(tcgetattr(fd, &tp_conf) > 0) {
           std::cout << "Error: tcgetattr not supported." << std::endl;
       }
      }
      else
      {
        std::cout << "Error: file descriptor not get." << std::endl;
      }
      return tp_conf;
    }

  protected:
    int fd;
    serial_rs485 rs485;
    termios tp;
};

template <class T>
class Set_serial_mode_test : public ::testing::Test
{
  public:
  virtual ~Set_serial_mode_test()
  {
    delete pParams;
    pParams = 0;
  }
  protected:
  Set_serial_mode_test() : pParams(0)
  {
    pParams = new T();
  }

  void SetUp()
  {
    pParams->SetUp();
  }
  void TearDown()
  {
    pParams->TearDown();
  }
  T * pParams;
};

class Set_serial_mode
{
public:
  Set_serial_mode()
  {
  }
  virtual ~Set_serial_mode(){}

  void SetUp()
  {
  }
  virtual void TearDown()
  {
  }
};

TYPED_TEST_CASE_P(Set_serial_mode_test);

TYPED_TEST_P(Set_serial_mode_test, setrs485)
{
  gchar * pOut;
  tty com(TTY);

  serial_rs485 rs485_expected = com.getrs485();
  rs485_expected.flags = SER_RS485_ENABLED | SER_RS485_RTS_ON_SEND;

  termios tp_expected = com.getTCSANOW();
  tp_expected.c_iflag |= IGNBRK;
  tp_expected.c_cflag |= CREAD;

  ASSERT_EQ(CallExecutable("/etc/config-tools/set_serial_mode rs485", &pOut), 0);
  ASSERT_TRUE(pOut != nullptr);
  g_free(pOut);

  serial_rs485 rs485 = com.getrs485();
  termios tp = com.getTCSANOW();

  EXPECT_EQ(rs485_expected, rs485);
  EXPECT_EQ(tp_expected, tp);
}

TYPED_TEST_P(Set_serial_mode_test, setrs232)
{
  gchar * pOut;
  tty com(TTY);

  serial_rs485 rs485_expected = com.getrs485();
  rs485_expected.flags &= (__u32)~(SER_RS485_ENABLED | SER_RS485_RTS_ON_SEND);

  termios tp_expected = com.getTCSANOW();
  tp_expected.c_iflag &= (tcflag_t)~IGNBRK;
  tp_expected.c_cflag &= (tcflag_t)~CREAD;

  ASSERT_EQ(CallExecutable("/etc/config-tools/set_serial_mode rs232", &pOut), 0);
  ASSERT_TRUE(pOut != nullptr);
  g_free(pOut);

  serial_rs485 rs485 = com.getrs485();
  termios tp = com.getTCSANOW();

  EXPECT_EQ(rs485_expected, rs485);
  EXPECT_EQ(tp_expected, tp);
}


REGISTER_TYPED_TEST_CASE_P( Set_serial_mode_test,
                            setrs485,
                            setrs232);


INSTANTIATE_TYPED_TEST_CASE_P(Serial_Target, Set_serial_mode_test, Set_serial_mode);
