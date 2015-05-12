#ifndef _PRTL_H_
#define _PRTL_H_

namespace prtl {

	class prtl_exception {
	      public:
		char *m_message;///< \brief takes the pointer to the error string.
		char *m_file;	///< \brief takes the pointer to the file string.
		int m_line;	///< \brief takes the line of the error.
		char *m_rcsid;	///< \brief takes the pointer to the rcsid string.

		prtl_exception(char *const message, char *const file = NULL, int line = 0, char *const rcsid = NULL)
		{
			m_message = message;
			m_file = file;
			m_line = line;
			m_rcsid = rcsid;
		}
	};

	template <class T> class array {
	      protected:
		T *m_data;
		unsigned int m_size;
	      public:
		array()
		{
			m_data = NULL;
			m_size = 0;
		}

		array(array &a)
		{
			m_data = NULL;
			m_size = 0;

			copy(a);
		}

		array(const unsigned int size)
		{
			m_data = NULL;
			m_size = 0;

			resize(size);
		}

		~array()
		{
			m_size = 0;
			if (m_data != NULL)
				delete [] m_data;
			m_data = NULL;
		}

		unsigned int size()
		{
			return m_size;
		}

		bool empty()
		{
			return m_size == 0;
		}

		void resize(const unsigned int count, const T &value = T())
		{
			T *temp;
			unsigned int i;

			if (count == m_size)
				return;

			temp = m_data;
			if (count != 0)
				m_data = new T[count];
			else
				m_data = NULL;

			if (m_data == NULL) {
				delete [] temp;
				throw prtl_exception("array[] not enough memory", __FILE__, __LINE__);
			}

			for (i = m_size; i < count; i++)
				m_data[i] = value;

			if (temp) {
				for (i = 0; i < ((count > m_size)?(m_size):(count)); i++)
					m_data[i] = temp[i];
			
				delete [] temp;
			}

			m_size = count;
		}

		T &operator [] (const unsigned int index)
		{
			if (index >= m_size)
				throw prtl_exception("array[] out of bounds", __FILE__, __LINE__);

			T *address;

			address = m_data + index;
			return *address;
		}

		void copy(array<T> &a)
		{
			if (a.size() > size())
				resize(a.size());

			for (unsigned int i = 0; i < size(); i++)
				m_data[i] = a[i];
		}
	};

}

#endif // _PRTL_H_
