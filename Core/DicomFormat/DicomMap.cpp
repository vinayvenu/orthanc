/**
 * Orthanc - A Lightweight, RESTful DICOM Store
 * Copyright (C) 2012 Medical Physics Department, CHU of Liege,
 * Belgium
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/


#include "DicomMap.h"

#include <stdio.h>
#include <memory>
#include "DicomString.h"
#include "../OrthancException.h"


namespace Orthanc
{
  static DicomTag patientTags[] =
  {
    DicomTag(0x0010, 0x0010), // PatientName
    DicomTag(0x0010, 0x0020), // PatientID
    DicomTag(0x0010, 0x0030), // PatientBirthDate
    DicomTag(0x0010, 0x0040), // PatientSex
    DicomTag(0x0010, 0x1000)  // OtherPatientIDs
    //DicomTag(0x0010, 0x1010), // PatientAge
    //DicomTag(0x0010, 0x1040)  // PatientAddress
  };

  static DicomTag studyTags[] =
  {
    DicomTag(0x0008, 0x0020), // StudyDate
    DicomTag(0x0008, 0x0030), // StudyTime
    DicomTag(0x0008, 0x0050), // AccessionNumber
    DicomTag(0x0008, 0x1030), // StudyDescription
    DicomTag(0x0020, 0x000d), // StudyInstanceUID
    DicomTag(0x0020, 0x0010)  // StudyID
    //DicomTag(0x0010, 0x1020), // PatientSize
    //DicomTag(0x0010, 0x1030)  // PatientWeight
  };

  static DicomTag seriesTags[] =
  {
    DicomTag(0x0008, 0x0021), // SeriesDate
    DicomTag(0x0008, 0x0031), // SeriesTime
    DicomTag(0x0008, 0x0060), // Modality
    DicomTag(0x0008, 0x0070), // Manufacturer
    DicomTag(0x0008, 0x1010), // StationName
    DicomTag(0x0008, 0x103e), // SeriesDescription
    //DicomTag(0x0010, 0x1080), // MilitaryRank
    DicomTag(0x0018, 0x0015), // BodyPartExamined
    DicomTag(0x0018, 0x0024), // SequenceName
    DicomTag(0x0018, 0x1030), // ProtocolName
    DicomTag(0x0020, 0x000e), // SeriesInstanceUID
    DicomTag(0x0020, 0x0011), // SeriesNumber
    DicomTag(0x0020, 0x1002), // ImagesInAcquisition
    DicomTag(0x0054, 0x0081)  // NumberOfSlices
  };

  static DicomTag instanceTags[] =
  {
    DicomTag(0x0008, 0x0012), // InstanceCreationDate
    DicomTag(0x0008, 0x0013), // InstanceCreationTime
    DicomTag(0x0008, 0x0018), // SOPInstanceUID
    DicomTag(0x0020, 0x0012), // AcquisitionNumber
    DicomTag(0x0020, 0x0013), // InstanceNumber
    DicomTag(0x0028, 0x0008), // NumberOfFrames
    DicomTag(0x0054, 0x1330)  // ImageIndex
  };




  void DicomMap::SetValue(uint16_t group, 
                          uint16_t element, 
                          DicomValue* value)
  {
    DicomTag tag(group, element);
    Map::iterator it = map_.find(tag);

    if (it != map_.end())
    {
      delete it->second;
      it->second = value;
    }
    else
    {
      map_.insert(std::make_pair(tag, value));
    }
  }

  void DicomMap::SetValue(DicomTag tag, 
                          DicomValue* value)
  {
    SetValue(tag.GetGroup(), tag.GetElement(), value);
  }




  void DicomMap::Clear()
  {
    for (Map::iterator it = map_.begin(); it != map_.end(); it++)
    {
      delete it->second;
    }

    map_.clear();
  }


  void DicomMap::ExtractTags(DicomMap& result,
                             const DicomTag* tags,
                             size_t count) const
  {
    result.Clear();

    for (unsigned int i = 0; i < count; i++)
    {
      Map::const_iterator it = map_.find(tags[i]);
      if (it != map_.end())
      {
        result.SetValue(it->first, it->second->Clone());
      }
    }
  }


  void DicomMap::ExtractPatientInformation(DicomMap& result) const
  {
    ExtractTags(result, patientTags, sizeof(patientTags) / sizeof(DicomTag));
  }

  void DicomMap::ExtractStudyInformation(DicomMap& result) const
  {
    ExtractTags(result, studyTags, sizeof(studyTags) / sizeof(DicomTag));
  }

  void DicomMap::ExtractSeriesInformation(DicomMap& result) const
  {
    ExtractTags(result, seriesTags, sizeof(seriesTags) / sizeof(DicomTag));
  }

  void DicomMap::ExtractInstanceInformation(DicomMap& result) const
  {
    ExtractTags(result, instanceTags, sizeof(instanceTags) / sizeof(DicomTag));
  }


  DicomMap* DicomMap::Clone() const
  {
    std::auto_ptr<DicomMap> result(new DicomMap);

    for (Map::const_iterator it = map_.begin(); it != map_.end(); it++)
    {
      result->map_.insert(std::make_pair(it->first, it->second->Clone()));
    }

    return result.release();
  }


  const DicomValue& DicomMap::GetValue(const DicomTag& tag) const
  {
    Map::const_iterator it = map_.find(tag);

    if (it == map_.end())
    {
      throw OrthancException("Inexistent tag");
    }
    else
    {
      return *it->second;
    }
  }


  void DicomMap::Remove(const DicomTag& tag) 
  {
    Map::iterator it = map_.find(tag);
    if (it != map_.end())
    {
      delete it->second;
      map_.erase(it);
    }
  }


  static void SetupFindTemplate(DicomMap& result,
                                const DicomTag* tags,
                                size_t count) 
  {
    result.Clear();

    for (size_t i = 0; i < count; i++)
    {
      result.SetValue(tags[i], "");
    }
  }

  void DicomMap::SetupFindPatientTemplate(DicomMap& result)
  {
    SetupFindTemplate(result, patientTags, sizeof(patientTags) / sizeof(DicomTag));
  }

  void DicomMap::SetupFindStudyTemplate(DicomMap& result)
  {
    SetupFindTemplate(result, studyTags, sizeof(studyTags) / sizeof(DicomTag));
    result.SetValue(DicomTag::ACCESSION_NUMBER, "");
    result.SetValue(DicomTag::PATIENT_ID, "");
  }

  void DicomMap::SetupFindSeriesTemplate(DicomMap& result)
  {
    SetupFindTemplate(result, seriesTags, sizeof(seriesTags) / sizeof(DicomTag));
    result.SetValue(DicomTag::ACCESSION_NUMBER, "");
    result.SetValue(DicomTag::PATIENT_ID, "");
    result.SetValue(DicomTag::STUDY_UID, "");
  }

  void DicomMap::SetupFindInstanceTemplate(DicomMap& result)
  {
    SetupFindTemplate(result, instanceTags, sizeof(instanceTags) / sizeof(DicomTag));
    result.SetValue(DicomTag::ACCESSION_NUMBER, "");
    result.SetValue(DicomTag::PATIENT_ID, "");
    result.SetValue(DicomTag::STUDY_UID, "");
    result.SetValue(DicomTag::SERIES_UID, "");
  }


  void DicomMap::CopyTagIfExists(const DicomMap& source,
                                 const DicomTag& tag)
  {
    if (source.HasTag(tag))
    {
      SetValue(tag, source.GetValue(tag));
    }
  }
}