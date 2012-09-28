
#pragma once

namespace Tries
{

class Trie
{
public:
   Trie(Trie * parentNode, bool isTerminal) : m_parentNode(parentNode), m_isTerminal(isTerminal)
   {
   }

   virtual ~Trie()
   {
   }

   bool  IsTerminal() const { return this->m_isTerminal; }
   const Trie* GetParent()  const { return this->m_parentNode; }

   virtual Trie* AddChild(char* word) = 0;
   virtual Trie* GetChild(char c) const = 0;
   virtual void  SetChild(char c, Trie* t) = 0;
   virtual bool  Lookup(char* word) const = 0;

   virtual char* ToString() const = 0;

   virtual char* Element() const = 0;

   bool HasChild(char c) const
   { 
      return this->GetChild(c) != NULL; 
   }

protected:
   const Trie * m_parentNode;
   bool   m_isTerminal;
};


class BasicTrie : public Tries::Trie
{
public:
   BasicTrie() : Trie(NULL, false)
   {
      // The root element has no char

      this->myChar = '\0';

      ::memset(this->childNodes, NULL, sizeof(Trie*)*26);

      assert(!this->IsTerminal());
   }

   virtual ~BasicTrie()
   {
      int i;

      // Delete any child nodes

      for(i = 0; i < 26; i++)
      {
         char c = 'a' + i;
         if (this->HasChild(c))
         {
            Trie * t = this->GetChild(c);

            delete t;
         }
      }
   }

   virtual char* Element() const
   {
      char* elt = new char[2];
      
      elt[0] = this->myChar;
      elt[1] = '\0';

      return elt;
   }

   virtual Trie* AddChild(char* word)
   {
      Trie * newTrie;
      char   c = word[0];

      if (c == '\0')
      {
         this->m_isTerminal = true;
         return this;
      }

      if (!this->HasChild(c))
      {
         newTrie = new BasicTrie(this, word);
      }
      else
      {
         newTrie = this->GetChild(c);
         newTrie = newTrie->AddChild(word+1);
      }

      this->SetChild(c, newTrie);

      return this;
   }

   virtual Trie * GetChild(char c) const
   {
      assert(islower(c));

      int index = c - 'a';

      return this->childNodes[index];
   }

   virtual void SetChild(char c, Trie * t)
   {
      assert(islower(c));

      int index = c - 'a';

      this->childNodes[index] = t;
   }

   virtual bool  Lookup(char* word) const
   {
      char   c = word[0];

      if (c == '\0')
      {
         return this->m_isTerminal;
      }

      if (!this->HasChild(c))
      {
         return false;
      }
      else
      {
         Trie* trie = this->GetChild(c);
         return trie->Lookup(word+1);
      }
   }

   virtual char* ToString() const
   {
      char* str;
      int length = 0;
      const Trie* iter = this;

      if (this->myChar == '\0')
      {
         str = new char[1];
         str[0] = '\0';
         return str;
      }

      while(iter != NULL)
      {
         length++;
         iter = iter->GetParent();
      }

      assert(length >= 2);

      str = new char[length];
      str[--length] = '\0';

      iter = this;

      while(static_cast<const Tries::BasicTrie *>(iter)->myChar != '\0')
      {
         str[--length] = static_cast<const Tries::BasicTrie *>(iter)->myChar;
         iter = iter->GetParent();
      }

      return str;
   }

protected:

   BasicTrie(Trie * parentNode, char* word) : Trie(parentNode, word[1] == '\0')
   {
      this->myChar = word[0];

      ::memset(this->childNodes, NULL, sizeof(Trie*)*26);

      if (this->IsTerminal()) return;

      this->AddChild(word+1);
   }

   Trie * childNodes[26];
   char   myChar;
};

} // Tries